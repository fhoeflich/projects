/*
 * Copyright (c) 2021,2023, BlackBerry Limited. All Rights Reserved.
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

#ifndef _IMX8_HW_VARIANT_H_INCLUDED
#define _IMX8_HW_VARIANT_H_INCLUDED


#include <aarch64/mx8mp.h>


#define GPIO_BANK_CNT            5
#define GPIO_PIN_CNT             32

#define GPIO_BASE_ARRAY \
    { \
        IMX_GPIO1_BASE, \
        IMX_GPIO2_BASE, \
        IMX_GPIO3_BASE, \
        IMX_GPIO4_BASE, \
        IMX_GPIO5_BASE, \
    }

#endif /*_IMX8_HW_VARIANT_H_INCLUDED*/

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/resmgr/imx8/aarch64/imx8mp.le/hw.h $ $Rev: 971300 $")
#endif
