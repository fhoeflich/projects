/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 * Copyright 2024, Ezurio LLC
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
#include <string.h>
#include <fcntl.h>
#include <hw/i2c.h>

/*
 * RV3028 RTC Serial Access Timekeeper
 */
#define RV3028_SEC             0
#define RV3028_MIN             1
#define RV3028_HOUR            2
#define RV3028_WDAY            3
#define RV3028_DATE            4
#define RV3028_MONTH           5
#define RV3028_YEAR            6
#define RV3028_STATUS_REG      14
#define RV3028_EEPROM_REG      0x37

/* STATUS reg bit */
#define RV3028_STATUS_PORF     0x01
#define RV3028_STATUS_EEBUSY   0x80

/* EEPROM BACKUP reg bit */
#define RV3028_EEPROM_FEDE     (1 << 4)
#define RV3028_EEPROM_LSM      (1 << 3)
#define RV3028_EEPROM_DSM      (1 << 2)

#define RV3028_SEC_MASK        0x7F
#define RV3028_MIN_MASK        0x7F
#define RV3028_HOUR_MASK       0x3F
#define RV3028_WDAY_MASK       0x07
#define RV3028_DATE_MASK       0x3F
#define RV3028_MONTH_MASK      0x1F
#define RV3028_YEAR_MASK       0xFF

#define RV3028_I2C_ADDRESS     (0x52)
#define RV3028_I2C_DEVNAME     "/dev/i2c2"

static int fd = -1;
static int slave = RV3028_I2C_ADDRESS;
static unsigned char starts_value = 0;

static int
rv3028_i2c_read(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[2], riov[2];
    i2c_sendrecv_t  hdr;

    if (starts_value & RV3028_STATUS_PORF) {
        fprintf(stderr, "%s: PORF detected, no time set\n", __func__);
        return (-1);
    }

    hdr.slave.addr  = slave;
    hdr.slave.fmt   = I2C_ADDRFMT_7BIT;
    hdr.send_len    = 1;
    hdr.recv_len    = num;
    hdr.stop        = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));

    SETIOV(&riov[0], &hdr, sizeof(hdr));
    SETIOV(&riov[1], val, num);

    return devctlv(fd, DCMD_I2C_SENDRECV, 2, 2, siov, riov, NULL);
}

static int
rv3028_i2c_write(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[3];
    i2c_send_t      hdr;
    int             ret;
    unsigned char   dsm;

    hdr.slave.addr  = slave;
    hdr.slave.fmt   = I2C_ADDRFMT_7BIT;
    hdr.len         = num + 1;
    hdr.stop        = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], val, num);

    ret = devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL);
    if (ret)
        return ret;

    /* Clear PORF status */
    starts_value &= ~RV3028_STATUS_PORF;
    rv3028_i2c_write(RV3028_STATUS_REG, &starts_value, 1);

    /* Enable DSM mode */
    dsm = RV3028_EEPROM_DSM | RV3028_EEPROM_FEDE;
    rv3028_i2c_write(RV3028_EEPROM_REG, &dsm, 1);

    return ret;
}

int
RTCFUNC(init,rv3028)(struct chip_loc *chip, char *argv[])
{
#define MAX_NAME  20
    char        i2c_dev[MAX_NAME] = RV3028_I2C_DEVNAME;
    int         opt;
    char        *value;
    char        *options;
    static char *opts[] = {
                    "i2c",          // I2C device name (default: /dev/i2c0)
                    "slave",        // slave address (default: 0x52)
                    NULL };

    /*
     * If the I2C device information is different from the default one,
     * specify the i2c device infomation in command line, for example:
     *    rtc rv3028 i2c=/dev/i2c3
     *    rtc rv3028 i2c=/dev/i2c3,slave=0x52
     */
    if (argv && argv[0]) {
        options = argv[0];
        while (options && *options != '\0') {
            if ((opt = getsubopt( &options, opts, &value)) == -1) {
                break;
            }

            switch (opt) {
                case 0:
                    strlcpy(i2c_dev, value, MAX_NAME);
                    break;

                case 1:
                    slave = strtol(value, 0, 0);
                    break;

                default:
                    break;
            }
        }
    }

    fd = open((const char *)i2c_dev, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Unable to open I2C device\n");
        return -1;
    }

    if (EOK != rv3028_i2c_read(RV3028_STATUS_REG, &starts_value, 1)) {
        fprintf(stderr, "RTC: rv3028_i2c_read() failed\n");
        return (-1);
    }

    return 0;
}

int
RTCFUNC(get,rv3028)(struct tm *tm, int cent_reg)
{
    unsigned char   date[7];

    if (EOK != rv3028_i2c_read(RV3028_SEC, date, 7)) {
        fprintf(stderr, "RTC: rv3028_i2c_read() failed\n");
        return (-1);
    }

    tm->tm_sec  = BCD2BIN(date[RV3028_SEC]   & RV3028_SEC_MASK);
    tm->tm_min  = BCD2BIN(date[RV3028_MIN]   & RV3028_MIN_MASK);
    tm->tm_hour = BCD2BIN(date[RV3028_HOUR]  & RV3028_HOUR_MASK);
    tm->tm_mday = BCD2BIN(date[RV3028_DATE]  & RV3028_DATE_MASK);
    tm->tm_mon  = BCD2BIN(date[RV3028_MONTH] & RV3028_MONTH_MASK) - 1;
    tm->tm_year = BCD2BIN(date[RV3028_YEAR]  & RV3028_YEAR_MASK) + 2000 - 1900;
    tm->tm_wday = BCD2BIN(date[RV3028_WDAY]  & RV3028_WDAY_MASK);

    return(0);
}

int
RTCFUNC(set,rv3028)(struct tm *tm, int cent_reg)
{
    unsigned char   date[7];

    date[RV3028_SEC]   = BIN2BCD(tm->tm_sec);
    date[RV3028_MIN]   = BIN2BCD(tm->tm_min);
    date[RV3028_HOUR]  = BIN2BCD(tm->tm_hour);
    date[RV3028_DATE]  = BIN2BCD(tm->tm_mday);
    date[RV3028_MONTH] = BIN2BCD(tm->tm_mon + 1);
    if ((tm->tm_year < 100) | (tm->tm_year > 199)) {
        fprintf(stderr, "RTC: year must be between 2000 and 2099\n");
        return (-1);
    }
    date[RV3028_YEAR] = BIN2BCD((tm->tm_year) % 100);
    date[RV3028_WDAY] = BIN2BCD(tm->tm_wday);

    if (EOK !=  rv3028_i2c_write(RV3028_SEC, date, 7)) {
        fprintf(stderr, "RTC: rv3028_i2c_write() failed\n");
        return (-1);
    }

    return(0);
}
