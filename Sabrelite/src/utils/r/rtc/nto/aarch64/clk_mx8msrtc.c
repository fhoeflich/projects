/*
 * Copyright 2012, 2022, 2023 BlackBerry Limited.
 * Copyright 2022 NXP
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
 */

#include "rtc.h"
#include <time.h>

/*
 * SNVS Secure Non Voltatile Storage
 */
#define MX8M_SNVS_BASE          0x020CC000
#define MX8M_SNVS_SIZE          0x4000

#define MX8M_SNVS_LPCR          0x38    /* LP Control Register */
#define MX8M_SNVS_LPSRTCMR      0x50    /* LP Secure Real Time Counter MSB Register */
#define MX8M_SNVS_LPSRTCLR      0x54    /* LP Secure Real Time Counter LSB Register */

/* Bit Definitions */
#define	MX8M_SNVS_LPCR_SRTC_ENV	(1 << 0)

/* Other defines */
#define	CNTR_TO_SECS_SH	        15	    /* Converts 47-bit counter to 32-bit seconds */

int
RTCFUNC(init,mx8msrtc)(struct chip_loc *chip_ptr, char *argv[])
{
    if (chip_ptr->phys == NIL_PADDR) {
        chip_ptr->phys = MX8M_SNVS_BASE;
    }
    if (chip_ptr->access_type == NONE) {
        chip_ptr->access_type = MEMMAPPED;
    }

    return MX8M_SNVS_SIZE;
}

int
RTCFUNC(get, mx8msrtc)(struct tm *tm_struct, int cent_reg)
{
    uint64_t read1, read2;
    time_t counter_sec = MX8M_SNVS_LPSRTCMR;

    do {
        /* MSB */
        read1 = chip_read32(MX8M_SNVS_LPSRTCMR);
        read1 <<= 32;

        /* LSB */
        read1 |= (uint64_t)(chip_read32(MX8M_SNVS_LPSRTCLR));

        /* MSB */
        read2 = chip_read32(MX8M_SNVS_LPSRTCMR);
        read2 <<= 32;

        /* LSB */
        read2 |= (uint64_t)(chip_read32(MX8M_SNVS_LPSRTCLR));

    //Loop while time inconsistent
    } while (read1 != read2);

	/* Convert 47-bit counter to 32-bit raw second count */
	counter_sec = (time_t) (read1 >> CNTR_TO_SECS_SH);

#ifdef  VERBOSE_SUPPORTED
    if (verbose) {
        printf("rtc read: %ld\n", counter_sec);
    }
#endif

    gmtime_r(&counter_sec, tm_struct);

    return(0);
}


int
RTCFUNC(set, mx8msrtc)(struct tm *tm_struct, int cent_reg)
{
    uint32_t lp_cr;
    time_t      t;

    t = mktime(tm_struct);

    /*
     *  mktime assumes local time.  We will subtract nd timezmezone
     */
    t -= timezone;

#ifdef  VERBOSE_SUPPORTED
    if (verbose) {
        printf("rtc write: %ld\n", t);
    }
#endif

    /* Disable RTC first */
    lp_cr = chip_read32(MX8M_SNVS_LPCR) & ~MX8M_SNVS_LPCR_SRTC_ENV;
    chip_write32(MX8M_SNVS_LPCR, lp_cr);
    while (chip_read32(MX8M_SNVS_LPCR) & MX8M_SNVS_LPCR_SRTC_ENV);

    /* Write 32-bit time to 47-bit timer, leaving 15 LSBs blank */
    chip_write32(MX8M_SNVS_LPSRTCLR, (unsigned)(t << CNTR_TO_SECS_SH));
    chip_write32(MX8M_SNVS_LPSRTCMR, (unsigned)(t >> (32 - CNTR_TO_SECS_SH)));

    /* Enable RTC again */
    chip_write32(MX8M_SNVS_LPCR, lp_cr | MX8M_SNVS_LPCR_SRTC_ENV);
    while (!(chip_read32(MX8M_SNVS_LPCR) & MX8M_SNVS_LPCR_SRTC_ENV));

    return(0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/utils/r/rtc/nto/aarch64/clk_mx8msrtc.c $ $Rev: 982741 $")
#endif
