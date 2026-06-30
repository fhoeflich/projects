/*
 * Copyright (c) 2021, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software. Free development
 * licenses are available for evaluation and non-commercial purposes. For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others. Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 */

#ifndef _LOG_H_INCLUDED
#define _LOG_H_INCLUDED

#include <sys/slog2.h>

#define _GPIO_SLOG(_level_, _format_, args...) \
    gpio_slogf((_level_), "%s[%u]: " _format_, __FUNCTION__, __LINE__, ##args)

#define GPIO_SLOG_DEBUG(_format_, args...)      _GPIO_SLOG(SLOG2_DEBUG1, _format_, ##args)
#define GPIO_SLOG_CRITICAL(_format_, args...)   _GPIO_SLOG(SLOG2_CRITICAL, _format_, ##args)
#define GPIO_SLOG_ERROR(_format_, args...)      _GPIO_SLOG(SLOG2_ERROR, _format_, ##args)
#define GPIO_SLOG_INFO(_format_, args...)       _GPIO_SLOG(SLOG2_INFO, _format_, ##args)

void gpio_slogf(uint8_t const severity, const char *const fmt, ...);

#endif /*_LOG_H_INCLUDED*/

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/resmgr/log.h $ $Rev: 932877 $")
#endif
