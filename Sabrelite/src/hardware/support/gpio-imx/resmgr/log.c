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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#include "log.h"

/* Maximum verbosity allowed to be logged */
#define SLOG_VERBOSITY_LEVEL        SLOG2_DEBUG2

/* Buffer to send messages to slogger2 */
static slog2_buffer_t slog2_buffer;

/**
 * @brief Initialize the slogger2 context for the resource manager
 *
 * @return int      0 on success, terminate the resource manager otherwise
 */
static int init_slog2(void)
{
    extern char *__progname;
    slog2_buffer_set_config_t config;

    memset(&config, 0, sizeof(slog2_buffer_set_config_t));
    config.buffer_set_name = __progname;            /* Use the program name */
    config.num_buffers = 1;                         /* Configure one slog buffer */
    config.verbosity_level = SLOG_VERBOSITY_LEVEL;  /* Allow maximum verbosity */
    config.buffer_config[0].buffer_name = "slog";   /* Default buffer name */
    config.buffer_config[0].num_pages = 1;          /* Use a 4KB buffer */

    /* Register the slog2 buffer */
    if (slog2_register(&config, &slog2_buffer, SLOG2_QUIET) < 0) {
        /* Exit upon fatal error */
        fprintf(stderr, "%s: Failed to initialize slog2 buffer: %s. Abort!\n",
            __progname, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

/**
 * @brief Log a formatted message in slog buffer
 *
 * @param severity      Verbosity level for the message
 * @param fmt           Formatted character string
 * @param ...
 */
void gpio_slogf(uint8_t const severity, const char *const fmt, ...)
{
    va_list arglist;

    va_start(arglist, fmt);

    if (severity <= SLOG_VERBOSITY_LEVEL) {
        if (slog2_buffer == NULL) {
            init_slog2();
        }
        if (slog2_buffer != NULL) {
            vslog2f(slog2_buffer, 0, severity, fmt, arglist);
        }
    }

    va_end(arglist);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/resmgr/log.c $ $Rev: 932877 $")
#endif
