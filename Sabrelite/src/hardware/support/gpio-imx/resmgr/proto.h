/*
 * Copyright (c) 2018,2021, QNX Software Systems. All Rights Reserved.
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

#ifndef _PROTO_H_INCLUDED
#define _PROTO_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <assert.h>

struct gpio_dev;
#define IOFUNC_ATTR_T       struct gpio_dev
#define THREAD_POOL_PARAM_T dispatch_context_t

#include <sys/iofunc.h>
#include <sys/procmgr.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <hw/dcmd_gpio_imx.h>

#define GPIO_RESMGR_PATH            "/dev/gpio"

/**
 * @brief GPIO attributes container for Resource Manager
 */
typedef struct gpio_dev {
    iofunc_attr_t       hdr;
    dispatch_t          *dpp;
    dispatch_context_t  *ctp;
    int                 id;
    void                *hdl;
} gpio_dev_t;

/*
 * Resource manager functions
 */
int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, iofunc_ocb_t *ocb);

/*
 * Platform-specific functions
 */
gpio_dev_t *hw_init(gpio_dev_t *dev);
void hw_fini(gpio_dev_t *dev);
int hw_cmd_write(void *hdl, gpio_devctl_t *devmsg);
int hw_cmd_read(void *hdl, gpio_devctl_t *devmsg);
int hw_cmd_set_output(void *hdl, gpio_devctl_t *devmsg);
int hw_cmd_set_input(void *hdl, gpio_devctl_t *devmsg);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/resmgr/proto.h $ $Rev: 932877 $")
#endif
