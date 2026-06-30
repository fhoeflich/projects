/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, 2020 QNX Software Systems.
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

/*
 * Dallas/Maxim DS3232 Serial RTC Time Setting Registers Address
 */
#define DS3232_SEC          0   /* 00-59 */
#define DS3232_MIN          1   /* 00-59 */
#define DS3232_HOUR         2   /* 0-1/00-23 */
#define DS3232_DAY          3   /* 01-07 */
#define DS3232_DATE         4   /* 01-31 */
#define DS3232_MONTH        5   /* 01-12 */
#define DS3232_YEAR         6   /* 00-99 */


#define DS3232_I2C_ADDRESS  (0xD0 >> 1)
#define DS3232_I2C_DEVNAME  "/dev/i2c1"


static int fd = -1;

static int
ds3232_i2c_read(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[2], riov[2];
    i2c_sendrecv_t  hdr;

    hdr.slave.addr = DS3232_I2C_ADDRESS;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.send_len = 1;
    hdr.recv_len = num;
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));

    SETIOV(&riov[0], &hdr, sizeof(hdr));
    SETIOV(&riov[1], val, num);

    return devctlv(fd, DCMD_I2C_SENDRECV, 2, 2, siov, riov, NULL);
}

static int
ds3232_i2c_write(unsigned char reg, unsigned char val[], unsigned char num)
{
    iov_t           siov[3];
    i2c_send_t      hdr;

    hdr.slave.addr = DS3232_I2C_ADDRESS;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.len = num + 1;
    hdr.stop = 1;

    SETIOV(&siov[0], &hdr, sizeof(hdr));
    SETIOV(&siov[1], &reg, sizeof(reg));
    SETIOV(&siov[2], val, num);

    return devctlv(fd, DCMD_I2C_SEND, 3, 0, siov, NULL, NULL);
}

int
RTCFUNC(init,ds3232)(struct chip_loc *chip, char *argv[])
{
    fd = open((argv && argv[0] && argv[0][0])?
            argv[0]: DS3232_I2C_DEVNAME, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Unable to open I2C device\n");
        return -1;
    }
    return 0;
}

int
RTCFUNC(get,ds3232)(struct tm *tm, int cent_reg)
{
    unsigned char   date[7];

    ds3232_i2c_read(DS3232_SEC, date, 7);

    tm->tm_sec  = BCD2BIN(date[DS3232_SEC] & 0x7f);
    tm->tm_min  = BCD2BIN(date[DS3232_MIN] & 0x7f);

	if ((date[DS3232_HOUR] & 0x40)) {
		/* the rtc is in 12 hour mode */
		int hour = BCD2BIN(date[DS3232_HOUR] & 0x1f);

		if ((date[DS3232_HOUR] & 0x20))
			tm->tm_hour = (hour == 12) ? 12 : hour + 12; /* pm */
		else
			tm->tm_hour = (hour == 12) ? 0 : hour;       /* am */

	} else {
		/* rejoice! the rtc is in 24 hour mode */
                tm->tm_hour = BCD2BIN(date[DS3232_HOUR] & 0x3f);
	}

    tm->tm_mday = BCD2BIN(date[DS3232_DATE] & 0x3f);
    tm->tm_mon  = BCD2BIN(date[DS3232_MONTH] & 0x1f) - 1;
    tm->tm_year = BCD2BIN(date[DS3232_YEAR] & 0xff);

	if ((date[DS3232_MONTH] & 0x80))
		tm->tm_year += 100;

    tm->tm_wday = BCD2BIN(date[DS3232_DAY] & 0x7) - 1;

    return(0);
}

int
RTCFUNC(set,ds3232)(struct tm *tm, int cent_reg)
{
    unsigned char   date[7];

	/*
	 * Note: this function will set the clock incorrectly after 2099
	 * And it sets the clock in 24 hour mode
	 */

    date[DS3232_SEC]   = BIN2BCD(tm->tm_sec);
    date[DS3232_MIN]   = BIN2BCD(tm->tm_min);
    date[DS3232_HOUR]  = BIN2BCD(tm->tm_hour);
    date[DS3232_DATE]  = BIN2BCD(tm->tm_mday);
    date[DS3232_MONTH] = BIN2BCD(tm->tm_mon + 1);
    date[DS3232_YEAR]  = BIN2BCD(tm->tm_year % 100);
    date[DS3232_DAY]   = BIN2BCD(tm->tm_wday + 1);

	if (tm->tm_year >= 100)
            date[DS3232_MONTH]|= 0x80;

    ds3232_i2c_write(DS3232_SEC, date, 7);

    return(0);
}

#ifdef RTCALARM

/*
 * Dallas/Maxim DS3232 Serial RTC Alarm 1 Setting Registers Address
 */
#define DS3232_A1_SEC       7           /* 00-59 */
#define DS3232_A1_MIN       8           /* 00-59 */
#define DS3232_A1_HOUR      9           /* 00-23 */
#define DS3232_A1_DAY_DATE  10          /* 01-07/01-31 */

/*
 * Dallas/Maxim DS3232 Serial RTC Alarm 2 Setting Registers Address
 */
#define DS3232_A2_MIN       11          /* 00-59 */
#define DS3232_A2_HOUR      12          /* 00-23 */
#define DS3232_A2_DAY_DATE  13          /* 01-07/01-31 */
#define DS3232_A_SET_MASK   (1 << 7)    /* Bit 7 on all A1 and A2 alarm registers is alarm mask */
#define DS3232_A_DD_MASK    (1 << 6)    /* Bit 6 on the date/day register is weekly/monthly alarm mask */

/*
 * Dallas/Maxim DS3232 Serial RTC Control Register Address
 */
#define DS3232_CTL          14          /* Control Register */
#define DS3232_A1E_MASK     (1 << 0)    /* Alarm 1 interrupt enable */
#define DS3232_A2E_MASK     (1 << 1)    /* Alarm 2 interrupt enable */
#define DS3232_INTCN_MASK   (1 << 2)    /* alarm interrupt/SQW  */

/*
 * Dallas/Maxim DS3232 Serial RTC Status Register Address
 */
#define DS3232_STATUS       15          /* Control/Status Register */
#define DS3232_A1F_MASK     (1 << 0)    /* Alarm 1 status flag */
#define DS3232_A2F_MASK     (1 << 1)    /* Alarm 2 status flag */

int
RTCFUNC(set_alarm,ds3232)(const char* alarm_time_str) {
    enum opt_index {
        ALARM_NUM = 0,  // alarm's instance
        TODH,           // Time of day - Hour
        TODM,           // Time of day - Minute
        TODS,           // Time of day - Second
        DOW,            // Day of week
        DOM,            // Date of Month
        END
    };

    int             ret = EOK;
    char            *options, *p;
    char            *delims = { "," };
    int             index;
    unsigned int    sec, minute, hour, day, date, alarm;
    int             alarm_weekly, alarm_monthly;

    // initialize the alarm number, day and date to invalid values
    sec = minute = hour = day = date = alarm = 0;

    if (NULL == alarm_time_str){
        fprintf(stderr, "No Alarm Option is received\n");
        ret = -1;
        return ret;
    }
    options = strdup (alarm_time_str);
    index = 0;
    // reset the flags for monthly and weekly alarms
    alarm_weekly = alarm_monthly = 0;
    p = strtok( options, delims );
    while( NULL != p ) {
        if (index >= END){
            fprintf(stderr, "The extra commandline options are ignored.\n");
            break;
        }
        switch (index) {
            case ALARM_NUM:
                alarm = (uint32_t)strtoul (p, 0, 0);
                delims = ":" ;
                if (alarm != 1 && alarm != 2){
                    fprintf(stderr, "invalid alarm number\n");
                    free (options);
                    return -1;
                }
                break;
            case TODH:
                hour = (uint32_t)strtoul (p, 0, 0);
                if (hour > 23){
                    fprintf(stderr, "invalid hour setting\n");
                    free (options);
                    return -1;
                }
                break;
            case TODM:
                minute = (uint32_t)strtoul (p, 0, 0);
                delims = "," ;
                if (minute > 59){
                    fprintf(stderr, "invalid minute setting\n");
                    free (options);
                    return -1;
                }
                break;
            case TODS:
                sec = (uint32_t)strtoul (p, 0, 0);
                if (sec > 59){
                    fprintf(stderr, "invalid seconds setting\n");
                    free (options);
                    return -1;
                }
                break;
            case DOW:
                day = (uint32_t)strtoul (p, 0, 0);
                if (day > 7){
                    fprintf(stderr, "invalid day of week setting\n");
                    free (options);
                    return -1;
                }
                if (0 != day){
                    alarm_weekly = 1;
                }
                break;
            case DOM:
                date = (uint32_t)strtoul (p, 0, 0);
                if (date > 31){
                    free (options);
                    return -1;
                }
                if (0 != date && !alarm_weekly){
                    alarm_monthly = 1;
                }
                break;
            default:
                fprintf(stderr, "Invalid number of Options.\n");
                break;
        }
        index++;
        p = strtok( NULL, delims );
    }
    free (options);
    /* Support for Daily, Weekly and Monthly alarms */
    /* Alarm 2 does not support seconds setting*/
    unsigned char alarm_setting[4] = {0};
    if (alarm == 1){
        // setting time (hour, minute and second using BIN2BCD sets the Alarm mask flags properly to 0)
        alarm_setting[0]=BIN2BCD(sec);
        alarm_setting[1]=BIN2BCD(minute);
        alarm_setting[2]=BIN2BCD(hour);
        if (alarm_weekly || alarm_monthly){
            if (alarm_weekly){
                alarm_setting[3]  = BIN2BCD(day);
                alarm_setting[3] |= DS3232_A_DD_MASK;
            } else {
                alarm_setting[3]  = BIN2BCD(date);
                alarm_setting[3] &= ~DS3232_A_DD_MASK;
            }
        } else { // A daily alarm should be set
            alarm_setting[3]  = BIN2BCD(7); // set the date/day register to a valid value; this value is ignored since the alarm is daily
            alarm_setting[3] |= DS3232_A_SET_MASK;
        }

        if (0 != ds3232_i2c_write(DS3232_A1_SEC, alarm_setting, 4)){
            fprintf(stderr, "unable to send the I2C alarm set message\n");
            ret = -1;
        }
    } else { // the second alarm setting
        alarm_setting[0]=BIN2BCD(minute);
        alarm_setting[1]=BIN2BCD(hour);
        if (alarm_weekly || alarm_monthly){
            if (alarm_weekly){
                alarm_setting[2]  = BIN2BCD(day);
                alarm_setting[2] |= DS3232_A_DD_MASK;
            } else {
                alarm_setting[2]  = BIN2BCD(date);
                alarm_setting[2] &= ~DS3232_A_DD_MASK;
            }
        } else { // A daily alarm should be set
            alarm_setting[2]  = BIN2BCD(7); // set the date/day register to a valid value; this value is ignored since the alarm is daily
            alarm_setting[2] |= DS3232_A_SET_MASK;
        }
        if (0 != ds3232_i2c_write(DS3232_A2_MIN, alarm_setting, 3)){
            fprintf(stderr, "unable to send the I2C alarm set message\n");
            ret = -1;
        }
    }

    return ret;
}

int
RTCFUNC(enable_alarm,ds3232)(const char* alarm_enable_str) {
    enum opt_index {
        ALARM_NUM = 0,  // alarm's instance
        EN,             // Enable/DisableB
        END
    };

    int             ret = EOK;
    char            *options, *p;
    char            *delims = { "," };
    unsigned int    index, alarm, enable_flag;

    if (NULL == alarm_enable_str){
        fprintf(stderr, "No Alarm Enable Option is received\n");
        ret = -1;
        return ret;
    }
    options = strdup (alarm_enable_str);
    index = alarm = enable_flag = 0;
    p = strtok( options, delims );
    while( NULL != p ) {
        if (index >= END){
            fprintf(stderr, "The extra commandline options are ignored.\n");
            break;
        }
        switch (index) {
            case ALARM_NUM:
                alarm = (uint32_t)strtoul (p, 0, 0);
                if (alarm != 1 && alarm != 2){
                    fprintf(stderr, "invalid alarm number\n");
                    free (options);
                    return -1;
                }
                break;
            case EN:
                enable_flag = (uint32_t)strtoul (p, 0, 0);
                if (0 != enable_flag && 1 != enable_flag){
                    fprintf(stderr, "invalid enable setting\n");
                    free (options);
                    return -1;
                }
                break;
            default:
                fprintf(stderr, "Invalid number of Options\n");
                break;
        }
        index++;
        p = strtok( NULL, delims );
    }
    free (options);
    unsigned char rtc_control[2];
    if ( 0 != ds3232_i2c_read(DS3232_CTL, rtc_control, 2)){
        fprintf(stderr, "unable to read RTC control/status registers.\n");
        return -1;
    }
    /* Enable / Disable Alarms */
    if (1 == alarm){
        if (1 == enable_flag) {
            rtc_control[0] |= DS3232_A1E_MASK;
            rtc_control[0] |= DS3232_INTCN_MASK;
        } else {
            rtc_control[0] &= ~DS3232_A1E_MASK;
        }
        rtc_control[1] &= ~DS3232_A1F_MASK;
    } else {
        if (1 == enable_flag) {
            rtc_control[0] |= DS3232_A2E_MASK;
            rtc_control[0] |= DS3232_INTCN_MASK;
        } else {
            rtc_control[0] &= ~DS3232_A2E_MASK;
        }
        rtc_control[1] &= ~DS3232_A2F_MASK;
    }
    if ( 0 != ds3232_i2c_write(DS3232_CTL, rtc_control, 2)){
        fprintf(stderr, "unable to write to RTC control/status registers\n");
        return -1;
    }
    return ret;
}
#endif //ifdef RTCALARM

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/utils/r/rtc/clk_ds3232.c $ $Rev: 936957 $")
#endif
