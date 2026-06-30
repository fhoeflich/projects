/*
 * $QNXLicenseC:
 * Copyright 2019, 2021, 2022, 2023 BlackBerry Limited.
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




#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __QNXNTO__

#include <sys/neutrino.h>
#include <hw/inout.h>

#define VERBOSE_SUPPORTED
#define SLOWADJUST
#define RTCALARM
#define _enable()   InterruptEnable()
#define _disable()  InterruptDisable()

#else

#include <sys/timers.h>
#include <sys/osinfo.h>
#include <i86.h>
#include <conio.h>

#define SLOWADJUST
#define VERBOSE_SUPPORTED
#define out8(a,b) outp(a,b)
#define in8(a) inp(a)
#define out16(a,b) outpw(a,b)
#define in16(a) inpw(a)
#define out32(a,b) outpd(a,b)
#define in32(a) inpd(a)

#define strtoull(a, b, c)	strtoul(a, b, c)
#define mmap_device_io(__len, __io)	(__io)
extern void *mmap_device_memory(void *__addr, size_t __len, int __prot, int __flags, unsigned __physical);
typedef unsigned	paddr64_t;

#endif

#define NIL_PADDR	(~(paddr64_t)0)

#define	UNSET		(-2)

struct chip_loc {
	paddr64_t	phys;
	unsigned	reg_shift;
	int		century_reg;
	enum {
			NONE = UNSET,
			IOMAPPED = 0,
			MEMMAPPED,
			RESMGR,
	} access_type;
	char		dev_write_addr;
	char		dev_read_addr;
	char		resmgr_path[PATH_MAX+1];
	char		*optstr;
};

struct rtc_desc {
        const char	*name;
	int		(*init)(struct chip_loc *, char **);
	int		(*get)(struct tm *, int);
	int		(*set)(struct tm *, int);
};

#ifdef RTCALARM

struct rtc_alarm_desc {
        const char	*name;
        int		(*set_alarm)(const char* alarm_time_str);
        int		(*enable_alarm)(const char* alarm_enable_str);
};
#endif

#ifdef VERBOSE_SUPPORTED
extern int verbose;
#endif

#define BIN2BCD(A)	(((((A) % 10000U)/1000U) << 12) + ((((A) % 1000U)/100U) << 8) + ((((A) % 100U)/10U) << 4) + ((A) % 10U))
#define BCD2BIN(A)	((((A) & 0xf000U) >> 12) * 1000U + (((A) & 0xf00U) >> 8) * 100U + (((A) & 0xf0U) >> 4) * 10U + ((A) & 0x0fU))

#define RTCFUNC(type,chip)	type##_##chip

extern int load_external_clock(const char *given_name, struct rtc_desc *clk);
extern char *query_clock_hw(struct chip_loc *);

extern unsigned	chip_read(unsigned off, unsigned size);
extern void chip_write(unsigned off, unsigned val, unsigned size);

#define chip_read8(off)			chip_read(off, 8)
#define chip_write8(off,val)	chip_write(off, val, 8)
#define chip_read16(off)			chip_read(off, 16)
#define chip_write16(off,val)	chip_write(off, val, 16)
#define chip_read32(off)			chip_read(off, 32)
#define chip_write32(off,val)	chip_write(off, val, 32)

extern int init_mc146818(struct chip_loc *chip, char *argv[]);
extern int get_mc146818(struct tm *tm, int cent_reg);
extern int set_mc146818(struct tm *tm, int cent_reg);

extern int init_ds1386(struct chip_loc *chip, char *argv[]);
extern int get_ds1386(struct tm *tm, int cent_reg);
extern int set_ds1386(struct tm *tm, int cent_reg);

extern int init_ds1743(struct chip_loc *chip, char *argv[]);
extern int get_ds1743(struct tm *tm, int cent_reg);
extern int set_ds1743(struct tm *tm, int cent_reg);

extern int init_ds15x1(struct chip_loc *chip, char *argv[]);
extern int get_ds15x1(struct tm *tm, int cent_reg);
extern int set_ds15x1(struct tm *tm, int cent_reg);

extern int init_ds3232(struct chip_loc *chip, char *argv[]);
extern int get_ds3232(struct tm *tm, int cent_reg);
extern int set_ds3232(struct tm *tm, int cent_reg);

#ifdef RTCALARM
extern int set_alarm_ds3232(const char* alarm_time_str);
extern int enable_alarm_ds3232(const char* alarm_enable_str);
#endif

extern int init_rtc72423(struct chip_loc *chip, char *argv[]);
extern int get_rtc72423(struct tm *tm, int cent_reg);
extern int set_rtc72423(struct tm *tm, int cent_reg);

extern int init_isl1208(struct chip_loc *chip, char *argv[]);
extern int get_isl1208(struct tm *tm, int cent_reg);
extern int set_isl1208(struct tm *tm, int cent_reg);

extern int init_m48t5x(struct chip_loc *chip, char *argv[]);
extern int get_m48t5x(struct tm *tm, int cent_reg);
extern int set_m48t5x(struct tm *tm, int cent_reg);

extern int init_m48t37(struct chip_loc *chip, char *argv[]);
extern int get_m48t37(struct tm *tm, int cent_reg);
extern int set_m48t37(struct tm *tm, int cent_reg);

extern int init_m41t00(struct chip_loc *chip, char *argv[]);
extern int get_m41t00(struct tm *tm, int cent_reg);
extern int set_m41t00(struct tm *tm, int cent_reg);

extern int init_m41t6x(struct chip_loc *chip, char *argv[]);
extern int get_m41t6x(struct tm *tm, int cent_reg);
extern int set_m41t6x(struct tm *tm, int cent_reg);

extern int init_s35390(struct chip_loc *chip, char *argv[]);
extern int get_s35390(struct tm *tm, int cent_reg);
extern int set_s35390(struct tm *tm, int cent_reg);

extern int init_mc9s08dz60(struct chip_loc *chip, char *argv[]);
extern int get_mc9s08dz60(struct tm *tm, int cent_reg);
extern int set_mc9s08dz60(struct tm *tm, int cent_reg);

extern int init_max8925(struct chip_loc *chip, char *argv[]);
extern int get_max8925(struct tm *tm, int cent_reg);
extern int set_max8925(struct tm *tm, int cent_reg);

extern int init_pcf2127at(struct chip_loc *chip, char *argv[]);
extern int get_pcf2127at(struct tm *tm, int cent_reg);
extern int set_pcf2127at(struct tm *tm, int cent_reg);

extern int init_rv3028(struct chip_loc *chip, char *argv[]);
extern int get_rv3028(struct tm *tm, int cent_reg);
extern int set_rv3028(struct tm *tm, int cent_reg);

extern int init_rx6110(struct chip_loc *chip, char *argv[]);
extern int get_rx6110(struct tm *tm, int cent_reg);
extern int set_rx6110(struct tm *tm, int cent_reg);

extern int init_tps65910(struct chip_loc *chip, char *argv[]);
extern int get_tps65910(struct tm *tm, int cent_reg);
extern int set_tps65910(struct tm *tm, int cent_reg);

extern int init_primecell(struct chip_loc *chip, char *argv[]);
extern int get_primecell(struct tm *tm, int cent_reg);
extern int set_primecell(struct tm *tm, int cent_reg);

extern int init_pmqcom(struct chip_loc *chip, char *argv[]);
extern int get_pmqcom(struct tm *tm, int cent_reg);
extern int set_pmqcom(struct tm *tm, int cent_reg);

#ifdef RTCALARM
extern int set_alarm_pmqcom(const char* alarm_time_str);
extern int enable_alarm_pmqcom(const char* alarm_enable_str);
#endif

#if (defined __aarch64__)
extern int init_mt2712(struct chip_loc *chip, char *argv[]);
extern int get_mt2712(struct tm *tm, int cent_reg);
extern int set_mt2712(struct tm *tm, int cent_reg);

extern int init_mcp7941x(struct chip_loc *chip_ptr, char *argv[]);
extern int get_mcp7941x(struct tm *tm_struct, int cent_reg);
extern int set_mcp7941x(struct tm *tm_struct, int cent_reg);

extern int init_mx8msrtc(struct chip_loc *chip_ptr, char *argv[]);
extern int get_mx8msrtc(struct tm *tm_struct, int cent_reg);
extern int set_mx8msrtc(struct tm *tm_struct, int cent_reg);

extern int init_mx93bbnsm(struct chip_loc *const chip_ptr, char *argv[]);
extern int get_mx93bbnsm(struct tm *const tm_struct, const int cent_reg);
extern int set_mx93bbnsm(struct tm *const tm_struct, const int cent_reg);

#if (defined VARIANT_imx8x)
extern int init_mx8sc(struct chip_loc *chip, char *argv[]);
extern int get_mx8sc(struct tm *tm, int cent_reg);
extern int set_mx8sc(struct tm *tm, int cent_reg);

#ifdef RTCALARM
extern int set_alarm_mx8sc(const char* alarm_time_str);
extern int enable_alarm_mx8sc(const char* alarm_enable_str);
#endif //def RTCALARM
#endif

extern int init_xzynq(struct chip_loc *chip, char *argv[]);
extern int get_xzynq(struct tm *tm, int cent_reg);
extern int set_xzynq(struct tm *tm, int cent_reg);
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/utils/r/rtc/rtc.h $ $Rev: 985987 $")
#endif
