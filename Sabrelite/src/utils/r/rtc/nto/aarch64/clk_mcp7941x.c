/*
 * Copyright (c) 2021, BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "rtc.h"
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <hw/i2c.h>


/*
 * MCP7941X RTC Serial Access Timekeeper
 */
#define MCP7941X_SEC           0
    #define MCP7941X_SEC_SEC_SHIFT                  0U
    #define MCP7941X_SEC_SEC_MASK                   0x0FU
    #define MCP7941X_SEC_10_SEC_SHIFT               4U
    #define MCP7941X_SEC_10_SEC_MASK                0x70U
    #define MCP7941X_SEC_ST_SHIFT                   7U
    #define MCP7941X_SEC_ST_MASK                    0x80U

#define MCP7941X_MIN           1
    #define MCP7941X_MIN_MIN_SHIFT                  0U
    #define MCP7941X_MIN_MIN_MASK                   0x0FU
    #define MCP7941X_MIN_10_MIN_SHIFT               4U
    #define MCP7941X_MIN_10_MIN_MASK                0x70U

#define MCP7941X_HOUR          2
    #define MCP7941X_HOUR_HOUR_SHIFT                0U
    #define MCP7941X_HOUR_HOUR_MASK                 0x0FU
    #define MCP7941X_HOUR_10_HOUR_SHIFT             4U
    #define MCP7941X_HOUR_10_HOUR_MASK              0x10U
    #define MCP7941X_HOUR_10_HOUR_AM_PM_SHIFT       5U
    #define MCP7941X_HOUR_10_HOUR_AM_PM_MASK        0x20U
    #define MCP7941X_HOUR_12_24_SHIFT               6U
    #define MCP7941X_HOUR_12_24_MASK                0x40U

#define MCP7941X_DAY           3
    #define MCP7941X_DAY_DAY_SHIFT                  0U
    #define MCP7941X_DAY_DAY_MASK                   0x07U
    #define MCP7941X_DAY_VBATEN_SHIFT               3U
    #define MCP7941X_DAY_VBATEN_MASK                0x08U
    #define MCP7941X_DAY_VBAT_SHIFT                 4U
    #define MCP7941X_DAY_VBAT_MASK                  0x10U
    #define MCP7941X_DAY_OSCON_SHIFT                5U
    #define MCP7941X_DAY_OSCON_MASK                 0x20U

#define MCP7941X_DATE          4
    #define MCP7941X_DATE_DATE_SHIFT                0U
    #define MCP7941X_DATE_DATE_MASK                 0x0FU
    #define MCP7941X_DATE_10_DATE_SHIFT             4U
    #define MCP7941X_DATE_10_DATE_MASK              0x30U

#define MCP7941X_MONTH         5
    #define MCP7941X_MONTH_MONTH_SHIFT              0U
    #define MCP7941X_MONTH_MONTH_MASK               0x0FU
    #define MCP7941X_MONTH_10_MONTH_SHIFT           4U
    #define MCP7941X_MONTH_10_MONTH_MASK            0x10U
    #define MCP7941X_MONTH_LP_SHIFT                 5U
    #define MCP7941X_MONTH_LP_MASK                  0x20U

#define MCP7941X_YEAR          6
    #define MCP7941X_YEAR_YEAR_SHIFT                0U
    #define MCP7941X_YEAR_YEAR_MASK                 0x0FU
    #define MCP7941X_YEAR_10_YEAR_SHIFT             4U
    #define MCP7941X_YEAR_10_YEAR_MASK              0xF0U

#define MCP7941X_CTRL_REG      7
    #define MCP7941X_CTRL_REG_RS_SHIFT              0U
    #define MCP7941X_CTRL_REG_RS_MASK               0x07U
    #define MCP7941X_CTRL_REG_EXTOSC_SHIFT          3U
    #define MCP7941X_CTRL_REG_EXTOSC_MASK           0x08U
    #define MCP7941X_CTRL_REG_ALM_SHIFT             4U
    #define MCP7941X_CTRL_REG_ALM_MASK              0x30U
    #define MCP7941X_CTRL_REG_SQWE_SHIFT            6U
    #define MCP7941X_CTRL_REG_SQWE_MAS              0x40U
    #define MCP7941X_CTRL_REG_OUT_SHIFT             7U
    #define MCP7941X_CTRL_REG_OUT_MASK              0x80U

#define MCP7941X_CALIBRATION   8
    #define MCP7941X_CALIBRATION_CALIBRATION_SHIFT  0U
    #define MCP7941X_CALIBRATION_CALIBRATION_MASK   0xFFU

#define MCP7941X_UNLOCK_ID     9
    #define MCP7941X_UNLOCK_ID_UNIQUE_ID_SHIFT      0U
    #define MCP7941X_UNLOCK_ID_UNIQUE_ID_MASK       0xFFU

#define MCP7941X_I2C_ADDRESS   (0x6F)
#define MCP7941X_I2C_DEVNAME   "/dev/i2c0"

//#define MCP7941X_DEBUG

static int fd = -1;
static uint32_t slave = MCP7941X_I2C_ADDRESS;

#ifdef MCP7941X_DEBUG
static void
mcp7941x_dump_reg(unsigned char date[])
{
    fprintf(stdout, "%s Seconds: 0x%X\n", __FUNCTION__, date[MCP7941X_SEC]);
    fprintf(stdout, "%s Minutes: 0x%X\n", __FUNCTION__, date[MCP7941X_MIN]);
    fprintf(stdout, "%s Hour: 0x%X\n", __FUNCTION__, date[MCP7941X_HOUR]);
    fprintf(stdout, "%s Week Day: 0x%X\n", __FUNCTION__, date[MCP7941X_DAY]);
    fprintf(stdout, "%s Month Date: 0x%X\n", __FUNCTION__, date[MCP7941X_DATE]);
    fprintf(stdout, "%s Month: 0x%X\n", __FUNCTION__, date[MCP7941X_MONTH]);
    fprintf(stdout, "%s Year: 0x%X\n", __FUNCTION__, date[MCP7941X_YEAR]);
}

static void
mcp7941x_dump_tm_struct(struct tm *tm_struct)
{
    fprintf(stdout, "%s Seconds: %d\n", __FUNCTION__, tm_struct->tm_sec);
    fprintf(stdout, "%s Minutes: %d\n", __FUNCTION__, tm_struct->tm_min);
    fprintf(stdout, "%s Hour: %d\n", __FUNCTION__, tm_struct->tm_hour);
    fprintf(stdout, "%s Week Day: %d\n", __FUNCTION__, tm_struct->tm_wday);
    fprintf(stdout, "%s Month Date: %d\n", __FUNCTION__, tm_struct->tm_mday);
    fprintf(stdout, "%s Month: %d\n", __FUNCTION__, tm_struct->tm_mon);
    fprintf(stdout, "%s Year: %d\n", __FUNCTION__, tm_struct->tm_year);
}
#endif

static int
mcp7941x_i2c_read(unsigned char reg, unsigned char val[], const unsigned char num)
{
    iov_t           siov[2], riov[2];
    i2c_sendrecv_t  hdr;

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
mcp7941x_i2c_write(unsigned char reg, unsigned char val[], const uint32_t num)
{
    iov_t           siov[3];
    i2c_send_t      hdr;

    hdr.slave.addr  = slave;
    hdr.slave.fmt   = I2C_ADDRFMT_7BIT;
    hdr.len         = num + 1;
    hdr.stop        = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], val, num);

    return devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL);
}

int
RTCFUNC(init,mcp7941x)(struct chip_loc *chip_ptr, char *argv[])
{
#define MAX_NAME  20
    char            i2c_dev[MAX_NAME];
    int             opt;
    unsigned char   data = 0;
    char            *value;
    char            *options;
    static char     *opts[] = {
            "i2c",      /* I2C device name (default: /dev/i2c0) */
            "slave",    /* slave address (default: 0x48) */
            NULL
            };

    strlcpy(i2c_dev, MCP7941X_I2C_DEVNAME, MAX_NAME);

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
                    slave = (uint32_t)strtol(value, NULL, 0);
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

    if (EOK != mcp7941x_i2c_read(MCP7941X_DAY, &data, 1)) {
        fprintf(stderr, "RTC: mcp7941x_i2c_read() failed\n");
        return (-1);
    }

    if ((data & MCP7941X_DAY_VBATEN_MASK) == 0) {
        data |= MCP7941X_DAY_VBATEN_MASK;
        if (EOK !=  mcp7941x_i2c_write(MCP7941X_DAY, &data, 1)) {
            fprintf(stderr, "RTC: mcp7941x_i2c_write() failed\n");
            return (-1);
        }
    }

    if (EOK != mcp7941x_i2c_read(MCP7941X_SEC, &data, 1)) {
        fprintf(stderr, "RTC: mcp7941x_i2c_read() failed\n");
        return (-1);
    }

    if ((data & MCP7941X_SEC_ST_MASK) == 0) {
        data |= MCP7941X_SEC_ST_MASK;
        if (EOK !=  mcp7941x_i2c_write(MCP7941X_SEC, &data, 1)) {
            fprintf(stderr, "RTC: mcp7941x_i2c_write() failed\n");
            return (-1);
        }
    }

    return 0;
}

int
RTCFUNC(get,mcp7941x)(struct tm *tm_struct, int cent_reg)
{
    unsigned char   date[7] = {0};
    unsigned char   tmp;

    if (EOK != mcp7941x_i2c_read(MCP7941X_SEC, date, sizeof(date))) {
        fprintf(stderr, "RTC: mcp7941x_i2c_read() failed\n");
        return (-1);
    }

#ifdef MCP7941X_DEBUG
    mcp7941x_dump_reg(date);
#endif

    tm_struct->tm_sec = (int)(BCD2BIN(date[MCP7941X_SEC] & MCP7941X_SEC_SEC_MASK));
    tmp = (unsigned char)BCD2BIN((date[MCP7941X_SEC] & MCP7941X_SEC_10_SEC_MASK) >> MCP7941X_SEC_10_SEC_SHIFT);
    tm_struct->tm_sec  += (tmp * 10);

    tm_struct->tm_min = (int)(BCD2BIN(date[MCP7941X_MIN] & MCP7941X_MIN_MIN_MASK));
    tmp = (unsigned char)BCD2BIN((date[MCP7941X_MIN] & MCP7941X_MIN_10_MIN_MASK) >> MCP7941X_MIN_10_MIN_SHIFT);
    tm_struct->tm_min  += (tmp * 10);

    tm_struct->tm_hour = (int)(BCD2BIN(date[MCP7941X_HOUR] & MCP7941X_HOUR_HOUR_MASK));
    if (date[MCP7941X_HOUR] & MCP7941X_HOUR_12_24_MASK)
    {
        /* Using 12 hour format */
        tmp = (unsigned char)BCD2BIN((date[MCP7941X_HOUR] & MCP7941X_HOUR_10_HOUR_MASK) >> MCP7941X_HOUR_10_HOUR_SHIFT);
        tm_struct->tm_hour += (tmp * 10);

        if (date[MCP7941X_HOUR] & MCP7941X_HOUR_10_HOUR_AM_PM_MASK) {
            /* Hour is in PM */
            tm_struct->tm_hour += 12;
        }
    } else {
        /* Using 24 hour format */
        tmp = (unsigned char)BCD2BIN((date[MCP7941X_HOUR] & (MCP7941X_HOUR_10_HOUR_MASK | MCP7941X_HOUR_10_HOUR_AM_PM_MASK))
            >> MCP7941X_HOUR_10_HOUR_SHIFT);
        tm_struct->tm_hour += (tmp * 10);
    }

    /* Day of the month */
    tm_struct->tm_mday = (int)(BCD2BIN(date[MCP7941X_DATE] & MCP7941X_DATE_DATE_MASK));
    tmp = (unsigned char)BCD2BIN((date[MCP7941X_DATE] & MCP7941X_DATE_10_DATE_MASK) >> MCP7941X_DATE_10_DATE_SHIFT);
    tm_struct->tm_mday  += (tmp * 10);

    tm_struct->tm_mon = (int)(BCD2BIN(date[MCP7941X_MONTH] & MCP7941X_MONTH_MONTH_MASK));
    tmp = (unsigned char)BCD2BIN((date[MCP7941X_MONTH] & MCP7941X_MONTH_10_MONTH_MASK) >> MCP7941X_MONTH_10_MONTH_SHIFT);
    tm_struct->tm_mon  += ((tmp * 10) - 1);

    tm_struct->tm_year = (int)(BCD2BIN(date[MCP7941X_YEAR] & MCP7941X_YEAR_YEAR_MASK));
    tmp = (unsigned char)BCD2BIN((date[MCP7941X_YEAR] & MCP7941X_YEAR_10_YEAR_MASK) >> MCP7941X_YEAR_10_YEAR_SHIFT);
    /* Start the year at 2000 */
    tm_struct->tm_year += (tmp * 10) + 100;

    /* Day of the week */
    tmp = (unsigned char)(date[MCP7941X_DAY] & MCP7941X_DAY_DAY_MASK);
    tm_struct->tm_wday = (int)(BCD2BIN(tmp) - 1);

#ifdef MCP7941X_DEBUG
    mcp7941x_dump_tm_struct(tm_struct);
#endif

    return(0);
}

int
RTCFUNC(set,mcp7941x)(struct tm *tm_struct, int cent_reg)
{
    unsigned char   date[7] = {0};
    unsigned char   tmp;

#ifdef MCP7941X_DEBUG
    mcp7941x_dump_tm_struct(tm_struct);
#endif

    if (EOK != mcp7941x_i2c_read(MCP7941X_SEC, date, sizeof(date))) {
        fprintf(stderr, "RTC: mcp7941x_i2c_read() failed\n");
        return (-1);
    }

    tmp = (unsigned char)(BIN2BCD((unsigned int)(tm_struct->tm_sec) / 10) << MCP7941X_SEC_10_SEC_SHIFT);
    tmp |= (BIN2BCD((unsigned int)(tm_struct->tm_sec) % 10U) << MCP7941X_SEC_SEC_SHIFT);
    date[MCP7941X_SEC] &= ~(MCP7941X_SEC_SEC_MASK | MCP7941X_SEC_10_SEC_MASK);
    date[MCP7941X_SEC] |= tmp;

    tmp = (unsigned char)(BIN2BCD((unsigned int)(tm_struct->tm_min) / 10) << MCP7941X_MIN_10_MIN_SHIFT);
    tmp |= (BIN2BCD((unsigned int)(tm_struct->tm_min) % 10) << MCP7941X_MIN_MIN_SHIFT);
    date[MCP7941X_MIN] = tmp;

    if (date[MCP7941X_HOUR] & MCP7941X_HOUR_12_24_MASK)
    {
        /* Using 12 hour format */
        if (date[MCP7941X_HOUR] & MCP7941X_HOUR_10_HOUR_AM_PM_MASK) {
            /* Hour is in PM */
            tmp = (unsigned char)(BIN2BCD(((unsigned int)(tm_struct->tm_hour) - 12) / 10) << MCP7941X_HOUR_10_HOUR_SHIFT);
            tmp |= (BIN2BCD(((unsigned int)(tm_struct->tm_hour) - 12) % 10) << MCP7941X_HOUR_HOUR_SHIFT);
            tmp |= MCP7941X_HOUR_10_HOUR_AM_PM_MASK;
        } else {
            tmp = (unsigned char)(BIN2BCD((unsigned int)(tm_struct->tm_hour) / 10) << MCP7941X_HOUR_10_HOUR_SHIFT);
            tmp |= (BIN2BCD((unsigned int)(tm_struct->tm_hour) % 10) << MCP7941X_HOUR_HOUR_SHIFT);
        }
        date[MCP7941X_HOUR] &= MCP7941X_HOUR_12_24_MASK;
        date[MCP7941X_HOUR] |= tmp;
    } else {
        /* Using 24 hour format */
        tmp = (unsigned char)(BIN2BCD((unsigned int)(tm_struct->tm_hour) / 10) << MCP7941X_HOUR_10_HOUR_SHIFT);
        tmp |= (BIN2BCD((unsigned int)(tm_struct->tm_hour) % 10) << MCP7941X_HOUR_HOUR_SHIFT);
        date[MCP7941X_HOUR] = tmp;
    }

    /* Day of the month */
    tmp = (unsigned char)(BIN2BCD((unsigned int)(tm_struct->tm_mday) / 10) << MCP7941X_DATE_10_DATE_SHIFT);
    tmp |= (BIN2BCD((unsigned int)(tm_struct->tm_mday) % 10) << MCP7941X_DATE_DATE_SHIFT);
    date[MCP7941X_DATE] = tmp;

    tmp = (unsigned char)(BIN2BCD(((unsigned int)(tm_struct->tm_mon) + 1) / 10) << MCP7941X_MONTH_10_MONTH_SHIFT);
    tmp |= (BIN2BCD(((unsigned int)(tm_struct->tm_mon) + 1) % 10) << MCP7941X_MONTH_MONTH_SHIFT);
    date[MCP7941X_MONTH] &= ~(MCP7941X_MONTH_MONTH_MASK | MCP7941X_MONTH_10_MONTH_MASK);
    date[MCP7941X_MONTH] |= tmp;

    if (tm_struct->tm_year<100)
    {
      fprintf(stderr, "RTC: year must be >=2000\n");
      return (-1);
    }

    /* Start the year at 2000 */
    tmp = (unsigned char)(BIN2BCD(((unsigned int)(tm_struct->tm_year) % 100) / 10) << MCP7941X_YEAR_10_YEAR_SHIFT);
    tmp |= (BIN2BCD(((unsigned int)(tm_struct->tm_year) % 100) % 10) << MCP7941X_YEAR_YEAR_SHIFT);
    date[MCP7941X_YEAR] = tmp;

    /* Day of the week */
    tmp = (unsigned char)(BIN2BCD((unsigned int)(tm_struct->tm_wday) + 1) << MCP7941X_DAY_DAY_SHIFT);
    date[MCP7941X_DAY] &= ~(MCP7941X_DAY_DAY_MASK);
    date[MCP7941X_DAY] |= tmp;

#ifdef MCP7941X_DEBUG
    mcp7941x_dump_reg(date);
#endif

    if (EOK !=  mcp7941x_i2c_write(MCP7941X_SEC, date, sizeof(date))) {
        fprintf(stderr, "RTC: mcp7941x_i2c_write() failed\n");
        return (-1);
    }
    return(0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/utils/r/rtc/nto/aarch64/clk_mcp7941x.c $ $Rev: 988459 $")
#endif
