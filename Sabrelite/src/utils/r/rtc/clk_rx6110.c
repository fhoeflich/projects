/*
 * $QNXLicenseC:
 * Copyright 2022 QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include "rtc.h"
#include <time.h>
#include <fcntl.h>
#include <hw/i2c.h>

/* Epson RX6110 RTC - Time Registers Address */
#define RX6110_REG_BASE     0x10
#define RX6110_NUM_REGS     7

/* Register offsets from RX6110_REG_BASE */
enum _rx6110_reg_offset {
    RX6110_SEC_OFFSET = 0,
    RX6110_MIN_OFFSET = 1,
    RX6110_HOUR_OFFSET = 2,
    RX6110_WDAY_OFFSET = 3,
    RX6110_MDAY_OFFSET = 4,
    RX6110_MONTH_OFFSET = 5,
    RX6110_YEAR_OFFSET = 6
};

#define RX6110_SEC_MASK     0x7f
#define RX6110_MIN_MASK     0x7f
#define RX6110_HOUR_MASK    0x3f
#define RX6110_MDAY_MASK    0x3f
#define RX6110_MONTH_MASK   0x1f
#define RX6110_YEAR_MASK    0xff

#define RX6110_YEAR_MIN     2000
#define RX6110_YEAR_MAX     2099

#define RX6110_I2C_DEVNAME_DEFAULT "/dev/i2c0"
#define RX6110_I2C_MAX_PATHNAME 512
#define RX6110_I2C_ADDR_DEFAULT 0x32
#define RX6110_OPT_DELIM ":"

static int fd = -1;
static uint8_t i2c_addr;

static int
rx6110_i2c_read(unsigned char reg_addr, unsigned char* const reg_vals, const unsigned char num_regs)
{
    iov_t siov[2], riov[2];
    i2c_sendrecv_t hdr;

    hdr.slave.addr = i2c_addr;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.send_len = 1;
    hdr.recv_len = num_regs;
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg_addr, sizeof(reg_addr));

    SETIOV(&riov[0], &hdr, sizeof(hdr));
    SETIOV(&riov[1], reg_vals, num_regs);

    return devctlv(fd, DCMD_I2C_SENDRECV, 2, 2, siov, riov, NULL);
}

static int
rx6110_i2c_write(unsigned char reg_addr, unsigned char* const reg_vals, const unsigned char num_regs)
{
    iov_t siov[3];
    i2c_send_t hdr;

    hdr.slave.addr = i2c_addr;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.len = (uint32_t)(num_regs + 1);
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg_addr, sizeof(reg_addr));
    SETIOV(&siov[2], reg_vals, num_regs);

    return devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL);
}

int
RTCFUNC(init,rx6110)(struct chip_loc *chip, char *argv[])
{
    char* opt;
    char i2c_path[RX6110_I2C_MAX_PATHNAME];

    /* Set defaults */
    i2c_addr = RX6110_I2C_ADDR_DEFAULT;
    strlcpy(i2c_path, RX6110_I2C_DEVNAME_DEFAULT, sizeof(i2c_path));

    /* Check for options in the form <I2C Path>:<I2C Address>. */
    if (argv && argv[0] && argv[0][0]) {
        opt = strtok(argv[0], RX6110_OPT_DELIM);
        if (opt != NULL) {
            strlcpy(i2c_path, opt, sizeof(i2c_path));
            opt = strtok(NULL, RX6110_OPT_DELIM);
            if (opt != NULL) {
                i2c_addr = (uint8_t)strtoul(opt, NULL, 0);
            }
        }
    }

    fd = open(i2c_path, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Failed to open I2C driver path ('%s')\n", i2c_path);
        return -1;
    }
    return 0;
}

/*
 * Note: this function will not work correctly after 2099.
 */
int
RTCFUNC(get,rx6110)(struct tm *tm, int cent_reg)
{
    unsigned char date[RX6110_NUM_REGS] = {0};
    unsigned char wday;
    int err;

    err = rx6110_i2c_read(RX6110_REG_BASE, date, RX6110_NUM_REGS);
    if (err != EOK) {
        fprintf(stderr, "Failed to communicate with I2C device at address 0x%x\n", i2c_addr);
        return -1;
    }

    tm->tm_sec = (int)BCD2BIN((unsigned)date[RX6110_SEC_OFFSET] & RX6110_SEC_MASK);
    tm->tm_min = (int)BCD2BIN((unsigned)date[RX6110_MIN_OFFSET] & RX6110_MIN_MASK);
    tm->tm_hour = (int)BCD2BIN((unsigned)date[RX6110_HOUR_OFFSET] & RX6110_HOUR_MASK);

    /* Weekday is stored as a bit-shifted value: (1<<0) == Sunday, (1<<1) == Monday, etc. */
    wday = (unsigned char)(date[RX6110_WDAY_OFFSET] & 0x7f);
    tm->tm_wday = 0;
    while (wday >>= 1) {
        tm->tm_wday++;
    }

    tm->tm_mday = (int)BCD2BIN((unsigned)date[RX6110_MDAY_OFFSET] & RX6110_MDAY_MASK);
    /* Convert the [1,12] BCD register value to tm_mon's "months since January" count. */
    tm->tm_mon = (int)BCD2BIN((unsigned)date[RX6110_MONTH_OFFSET] & RX6110_MONTH_MASK) - 1;
    /*
     * Convert the [0,99] BCD register value to tm_year's "years since 1900" count.
     * Note that this will only work from 2000 to 2099.
     */
    tm->tm_year = (int)BCD2BIN((unsigned)date[RX6110_YEAR_OFFSET] & RX6110_YEAR_MASK) + 100;

    return 0;
}

/*
 * Note: this function will not work correctly after 2099.
 */
int
RTCFUNC(set,rx6110)(struct tm *tm, int cent_reg)
{
    unsigned char date[RX6110_NUM_REGS] = {0};
    int err = 0;

    if (((tm->tm_year + 1900) < RX6110_YEAR_MIN) || ((tm->tm_year + 1900) > RX6110_YEAR_MAX)) {
        fprintf(stderr, "Failed to update the RTC time. This RTC device only supports year values between %d and %d.\n",
            RX6110_YEAR_MIN, RX6110_YEAR_MAX);
        return -1;
    }

    date[RX6110_SEC_OFFSET] = (unsigned char)(BIN2BCD((unsigned)tm->tm_sec) & RX6110_SEC_MASK);
    date[RX6110_MIN_OFFSET] = (unsigned char)(BIN2BCD((unsigned)tm->tm_min) & RX6110_MIN_MASK);
    date[RX6110_HOUR_OFFSET] = (unsigned char)(BIN2BCD((unsigned)tm->tm_hour) & RX6110_HOUR_MASK);
    /* Weekday is stored as a bit-shifted value: (1<<0) == Sunday, (1<<1) == Monday, etc. */
    date[RX6110_WDAY_OFFSET] = (unsigned char)(1L << tm->tm_wday);
    date[RX6110_MDAY_OFFSET] = (unsigned char)(BIN2BCD((unsigned)tm->tm_mday) & RX6110_MDAY_MASK);
    /* Convert tm_mon's "months since January" count to a [1,12] BCD value. */
    date[RX6110_MONTH_OFFSET] = (unsigned char)(BIN2BCD((unsigned)tm->tm_mon + 1) & RX6110_MONTH_MASK);
    /* tm_year counts the "years since 1900" so mod out 100 to calculate the 20xx value. */
    date[RX6110_YEAR_OFFSET] = (unsigned char)(BIN2BCD((unsigned)tm->tm_year % 100) & RX6110_YEAR_MASK);

    err = rx6110_i2c_write(RX6110_REG_BASE, date, RX6110_NUM_REGS);
    if (err != EOK) {
        fprintf(stderr, "Failed to communicate with I2C device at address 0x%x\n", i2c_addr);
        return -1;
    }

    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/utils/r/rtc/clk_rx6110.c $ $Rev: 985987 $")
#endif
