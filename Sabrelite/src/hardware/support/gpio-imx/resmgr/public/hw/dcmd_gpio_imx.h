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

#ifndef _HW_DCMD_GPIO_IMX_H_INCLUDED
#define _HW_DCMD_GPIO_IMX_H_INCLUDED

/**
 * @brief Set the GPIO pin to input
 */
typedef struct {
        uint8_t        bank;    /* [in] GPIO bank */
        uint8_t        pin;     /* [in] GPIO pin */
} gpio_devctl_input_t;

/**
 * @brief Set the GPIO pin to output with a specific state
 */
typedef struct {
        uint8_t        bank;    /* [in] GPIO bank */
        uint8_t        pin;     /* [in] GPIO pin */
        uint8_t        data;    /* [in] GPIO output state */
} gpio_devctl_output_t;

/**
 * @brief Get the GPIO pin state
 * @note Works with input or output pins
 */
typedef struct {
        uint8_t        bank;    /* [in] GPIO bank */
        uint8_t        pin;     /* [in] GPIO pin */
        uint8_t        data;    /* [out] GPIO input state */
} gpio_devctl_read_t;

/**
 * @brief Set the GPIO pin output state
 * @note Works only with output pins
 */
typedef struct {
        uint8_t        bank;    /* [in] GPIO bank */
        uint8_t        pin;     /* [in] GPIO pin */
        uint8_t        data;    /* [in] GPIO output state */
} gpio_devctl_write_t;

/**
 * @brief Union of all the supported devctl messages
 */
typedef union {
        gpio_devctl_input_t                 cmd_set_input;
        gpio_devctl_output_t                cmd_set_output;
        gpio_devctl_read_t                  cmd_read;
        gpio_devctl_write_t                 cmd_write;
} gpio_devctl_t;

#define GPIO_SET_INPUT                     0
#define GPIO_SET_OUTPUT                    1
#define GPIO_READ                          2
#define GPIO_WRITE                         3
#define DCMD_GPIO_SET_INPUT                __DIOT(_DCMD_MISC, GPIO_SET_INPUT, gpio_devctl_input_t)
#define DCMD_GPIO_SET_OUTPUT               __DIOT(_DCMD_MISC, GPIO_SET_OUTPUT, gpio_devctl_output_t)
#define DCMD_GPIO_READ                     __DIOTF(_DCMD_MISC, GPIO_READ, gpio_devctl_read_t)
#define DCMD_GPIO_WRITE                    __DIOTF(_DCMD_MISC, GPIO_WRITE, gpio_devctl_write_t)

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/resmgr/public/hw/dcmd_gpio_imx.h $ $Rev: 932877 $")
#endif
