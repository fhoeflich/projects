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

#include <devctl.h>
#include <sys/iomsg.h>

#include "proto.h"
#include "log.h"


/**
 * @brief Check command permissions against the device open flag
 *
 * @param cmd       Devctl command
 * @param oflag     Device open flag
 * @return int      -1 on permission violation, 0 otherwise
 */
static int io_devctl_permissions(int cmd, int oflag)
{
    switch (cmd) {

        /* Check commands for read permissions */
        case DCMD_GPIO_READ:
            if((oflag & _IO_FLAG_RD) == 0) {
                return -1;
            }
            break;

        /* Check commands for write permissions */
        case DCMD_GPIO_SET_INPUT:
            /*FALLTHRU*/
        case DCMD_GPIO_SET_OUTPUT:
            /*FALLTHRU*/
        case DCMD_GPIO_WRITE:
            if((oflag & _IO_FLAG_WR) == 0) {
                return -1;
            }
            break;

        /* Disallow unsupported commands */
        default:
            return -1;
    }
    return 0;
}

/**
 * @brief Implement the devctl functionality for the resource manager
 *
 * @param ctp       Resource manager context
 * @param msg       Message container
 * @param ocb       Open control block
 * @return int      errno
 */
int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, iofunc_ocb_t *ocb)
{
    int status;
    gpio_dev_t *dev;
    gpio_devctl_t *devmsg;

    /* Execute the default devctl routine */
    status = iofunc_devctl_default(ctp, msg, ocb);
    if (status != _RESMGR_DEFAULT) {
        return status;
    }

    /* Verify that the device was opened with the proper permissions */
    status = io_devctl_permissions(msg->i.dcmd, ocb->ioflag);
    if (status != 0) {
        GPIO_SLOG_ERROR("Permission denied for command 0x%x", msg->i.dcmd);
        return EPERM;
    }

    /* Get device pointer */
    dev = ocb->attr;
    /* Get device data pointer */
#if _NTO_VERSION <= 710
    devmsg = _DEVCTL_DATA(msg->i);
#else
    devmsg = _IO_INPUT_PAYLOAD(msg);
#endif

    /* Process the devctl command */
    switch (msg->i.dcmd) {

        case DCMD_GPIO_SET_INPUT:
            status = hw_cmd_set_input(dev->hdl, devmsg);
            break;

        case DCMD_GPIO_SET_OUTPUT:
            status = hw_cmd_set_output(dev->hdl, devmsg);
            break;

        case DCMD_GPIO_READ:
            status = hw_cmd_read(dev->hdl, devmsg);
            if (status < 0) break;
            msg->o.ret_val = 0;
            return _RESMGR_PTR(ctp, &msg->o,sizeof(msg->o) + sizeof(gpio_devctl_read_t));

        case DCMD_GPIO_WRITE:
            status = hw_cmd_write(dev->hdl, devmsg);
            if (status < 0) break;
            msg->o.ret_val = 0;
            return _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + sizeof(gpio_devctl_write_t));

        default:
            /* Unsupported command */
            return ENOSYS;
    }

    /* Handle failures of the the devctl commands */
    if (status < 0) {
        return EINVAL;
    }

    _RESMGR_STATUS(ctp, 0);
    return _RESMGR_NPARTS(0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/resmgr/devctl.c $ $Rev: 932877 $")
#endif
