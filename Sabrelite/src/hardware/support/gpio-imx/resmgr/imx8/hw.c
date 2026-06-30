/*
 * Copyright (c) 2018,2021,2023, BlackBerry Limited. All Rights Reserved.
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

#include <sys/neutrino.h>
#include <sys/mman.h>
#include <aarch64/imx8_common/imx_gpio.h>

#include "proto.h"
#include "log.h"
#include "hw.h"


/**
 * @brief GPIO bank information
 *
 * Container to store all the information about a specific GPIO bank
 */
typedef struct {
    paddr64_t       paddr;      /* Physical base address of the GPIO bank registers */
    uint8_t         *vaddr;     /* Virtual base address of the GPIO bank registers */
    size_t          size;       /* Length in bytes of the GPIO bank registers */
} gpio_bank_t;

/**
 * @brief GPIO Context Structure
 *
 * Container to store all the GPIO properties
 */
typedef struct {
    size_t          cnt;        /* Number of supported GPIO banks */
    gpio_bank_t     *bank;      /* GPIO bank array */
} gpio_context_t;

/**
 * @brief Physical address array of all the supported GPIO banks
 *
 */
static const paddr64_t gpio_base[GPIO_BANK_CNT] = GPIO_BASE_ARRAY;


/*
 * Local API
 */


/**
 * @brief Read 32-bit GPIO register
 *
 * @param bank          GPIO bank address
 * @param reg           Register offset
 * @return uint32_t     Register value
 */
static uint32_t gpio_reg_read(uint8_t *bank, uint8_t reg)
{
    assert(bank != NULL);
    return *(volatile uint32_t*)(bank + reg);
}

/**
 * @brief Write 32-bit GPIO register
 *
 * @param bank          GPIO bank address
 * @param reg           Register offset
 * @param data          Value to write
 */
static void gpio_reg_write(uint8_t *bank, uint8_t reg, uint32_t data)
{
    assert(bank != NULL);
    *(volatile uint32_t*)(bank + reg) = data;
}

/**
 * @brief Write 32-bit GPIO register with mask and data
 *
 * @param bank          GPIO bank address
 * @param reg           Register offset
 * @param mask          Mask for the value to write
 * @param data          Value to write
 */
static void gpio_reg_write_mask(uint8_t *bank, uint8_t reg, uint32_t mask, uint32_t data)
{
    gpio_reg_write(bank, reg, (gpio_reg_read(bank, reg) & ~mask) | (data & mask));
}


/*
 * GPIO Resource Manager API
 */


/**
 * @brief Initialize the platform's GPIOs
 *
 * @param dev           GPIO device container
 * @return gpio_dev_t*  GPIO device container on success, NULL otherwise
 */
gpio_dev_t *hw_init(gpio_dev_t *dev)
{
    unsigned int    i;
    gpio_context_t  *gpio_ctx;

    /* Sanity check */
    if (dev == NULL) {
        return NULL;
    }

    /* Request I/O privileges */
    if (ThreadCtl(_NTO_TCTL_IO, 0) == -1) {
        GPIO_SLOG_ERROR("Unable to obtain I/O privileges");
        return NULL;
    }

    /* Create the GPIO context */
    gpio_ctx = calloc(1, sizeof(gpio_context_t));
    if (gpio_ctx == NULL) {
        GPIO_SLOG_ERROR("Failed to create GPIO context");
        return NULL;
    }
    /* Assign GPIO bank count */
    gpio_ctx->cnt = min(GPIO_BANK_CNT, sizeof(gpio_base)/sizeof(gpio_base[0]));

    /* Allocate the GPIO bank array */
    gpio_ctx->bank = calloc(gpio_ctx->cnt, sizeof(gpio_bank_t));
    if (gpio_ctx->bank == NULL) {
        GPIO_SLOG_ERROR("Failed to create the GPIO bank array");
        free(gpio_ctx);
        return NULL;
    }

    /* Map all the GPIO registers into memory */
    for(i = 0; i < gpio_ctx->cnt; i++)
    {
        /* Catch invalid entry */
        assert(gpio_base[i] != 0);

        /* Assign the GPIO bank address and size */
        gpio_ctx->bank[i].paddr = gpio_base[i];
        gpio_ctx->bank[i].size = IMX_GPIO_SIZE;

        /* Map the GPIO register into memory */
        gpio_ctx->bank[i].vaddr = mmap_device_memory(NULL, gpio_ctx->bank[i].size,
            (PROT_READ | PROT_WRITE | PROT_NOCACHE), 0, gpio_ctx->bank[i].paddr);
        if (gpio_ctx->bank[i].vaddr == MAP_FAILED) {
            GPIO_SLOG_ERROR("Failed map to the GPIO bank #%d", i);
            goto Error;
        }
    }

    dev->hdl = gpio_ctx;
    return dev;

 Error:
    for(i = 0; i < gpio_ctx->cnt; i++) {

        if ((gpio_ctx->bank[i].vaddr != MAP_FAILED) && (gpio_ctx->bank[i].vaddr != NULL)) {
            munmap_device_memory(gpio_ctx->bank[i].vaddr, gpio_ctx->bank[i].size);
        }
    }
    free(gpio_ctx->bank);
    free(gpio_ctx);
    return NULL;
}

/**
 * @brief Release the platform resources
 *
 * @param dev       GPIO device container
 */
void hw_fini(gpio_dev_t __attribute__((unused)) *dev)
{
    /* Clean-up and unmapping handled by the kernel */
    return;
}

/**
 * @brief Read the state of the specified GPIO pin
 *
 * @note The physical pin must be configured as a GPIO in the IO multiplexer
 *       (IOMUX) during startup.
 *
 * @param hdl       GPIO context container
 * @param devmsg    devctl message buffer
 * @return int      0 on success, -1 otherwise
 */
int hw_cmd_read(void *hdl, gpio_devctl_t *devmsg)
{
    gpio_context_t  *gpio_ctx;
    uint8_t         pin_id;
    uint8_t         bank_id;
    uint32_t        reg_data;

    /* Sanity check */
    if ((hdl == NULL) || (devmsg == NULL)) {
        return -1;
    }
    gpio_ctx = hdl;

    pin_id = devmsg->cmd_read.pin;
    bank_id = devmsg->cmd_read.bank;

    if (pin_id >= GPIO_PIN_CNT) {
        GPIO_SLOG_ERROR("Pin %d is invalid", pin_id);
        return -1;
    } else if (bank_id >= gpio_ctx->cnt) {
        GPIO_SLOG_ERROR("Bank %d is invalid", bank_id);
        return -1;
    }

    reg_data = gpio_reg_read(gpio_ctx->bank[bank_id].vaddr, IMX_GPIO_DR);
    reg_data &= (uint32_t)(1 << pin_id);
    devmsg->cmd_read.data = (reg_data == 0) ? (0) : (1);

    return 0;
}

/**
 * @brief Write the specified state to the specified GPIO pin
 *
 * @note The physical pin must be configured as a GPIO in the IO multiplexer
 *       (IOMUX) during startup.
 *
 * @param hdl       GPIO context container
 * @param devmsg    devctl message buffer
 * @return int      0 on success, -1 otherwise
 */
int hw_cmd_write(void *hdl, gpio_devctl_t *devmsg)
{
    gpio_context_t  *gpio_ctx;
    uint8_t         pin_id;
    uint8_t         bank_id;
    uint8_t         pin_state;
    uint32_t        pin_mask;
    uint32_t        reg_data;

    /* Sanity check */
    if ((hdl == NULL) || (devmsg == NULL)) {
        return -1;
    }
    gpio_ctx = hdl;

    pin_id = devmsg->cmd_write.pin;
    bank_id = devmsg->cmd_write.bank;
    pin_state = devmsg->cmd_write.data;

    if (pin_id >= GPIO_PIN_CNT) {
        GPIO_SLOG_ERROR("Pin %d is invalid", pin_id);
        return -1;
    } else if (bank_id >= gpio_ctx->cnt) {
        GPIO_SLOG_ERROR("Bank %d is invalid", bank_id);
        return -1;
    }
    pin_mask = (uint32_t)(1 << pin_id);

    /* Check the configuration of the GPIO direction */
    reg_data = gpio_reg_read(gpio_ctx->bank[bank_id].vaddr, IMX_GPIO_GDIR);
    if ((reg_data & pin_mask) == 0) {
        GPIO_SLOG_ERROR("GPIO pin %d is set as input: Invalid for write operations!", pin_id);
        return -1;
    }

    if (pin_state == 0) {
        /* Set pin to ouput low */
        gpio_reg_write_mask(gpio_ctx->bank[bank_id].vaddr, IMX_GPIO_DR, pin_mask, 0);
    } else {
        /* Set pin to output high */
        gpio_reg_write_mask(gpio_ctx->bank[bank_id].vaddr, IMX_GPIO_DR, pin_mask, pin_mask);
    }

    return 0;
}

/**
 * @brief Set the specified GPIO pin to input
 *
 * @note The physical pin must be configured as a GPIO in the IO multiplexer
 *       (IOMUX) during startup.
 *
 * @param hdl       GPIO context container
 * @param devmsg    devctl message buffer
 * @return int      0 on success, -1 otherwise
 */
int hw_cmd_set_input(void *hdl, gpio_devctl_t *devmsg)
{
    gpio_context_t  *gpio_ctx;
    uint8_t         pin_id;
    uint8_t         bank_id;
    uint32_t        pin_mask;

    /* Sanity check */
    if ((hdl == NULL) || (devmsg == NULL)) {
        return -1;
    }
    gpio_ctx = hdl;

    pin_id = devmsg->cmd_set_input.pin;
    bank_id = devmsg->cmd_set_input.bank;

    if (pin_id >= GPIO_PIN_CNT) {
        GPIO_SLOG_ERROR("Pin %d is invalid", pin_id);
        return -1;
    } else if (bank_id >= gpio_ctx->cnt) {
        GPIO_SLOG_ERROR("Bank %d is invalid", bank_id);
        return -1;
    }
    pin_mask = (uint32_t)(1 << pin_id);

    /* Set pin to input */
    gpio_reg_write_mask(gpio_ctx->bank[bank_id].vaddr, IMX_GPIO_GDIR, pin_mask, 0);

    return 0;
}

/**
 * @brief Set the specified GPIO pin to output
 *
 * @note The physical pin must be configured as a GPIO in the IO multiplexer
 *       (IOMUX) during startup.
 *
 * @param hdl       GPIO context container
 * @param devmsg    devctl message buffer
 * @return int      0 on success, -1 otherwise
 */
int hw_cmd_set_output(void *hdl, gpio_devctl_t *devmsg)
{
    gpio_context_t  *gpio_ctx;
    uint8_t         pin_id;
    uint8_t         bank_id;
    uint8_t         pin_state;
    uint32_t        pin_mask;

    /* Sanity check */
    if ((hdl == NULL) || (devmsg == NULL)) {
        return -1;
    }
    gpio_ctx = hdl;

    pin_id = devmsg->cmd_set_output.pin;
    bank_id = devmsg->cmd_set_output.bank;
    pin_state = devmsg->cmd_set_output.data;

    if (pin_id >= GPIO_PIN_CNT) {
        GPIO_SLOG_ERROR("Pin %d is invalid", pin_id);
        return -1;
    } else if (bank_id >= gpio_ctx->cnt) {
        GPIO_SLOG_ERROR("Bank %d is invalid", bank_id);
        return -1;
    }
    pin_mask = (uint32_t)(1 << pin_id);

    if (pin_state == 0) {
        /* Set pin to low */
        gpio_reg_write_mask(gpio_ctx->bank[bank_id].vaddr, IMX_GPIO_DR, pin_mask, 0);
    } else {
        /* Set pin to high */
        gpio_reg_write_mask(gpio_ctx->bank[bank_id].vaddr, IMX_GPIO_DR, pin_mask, pin_mask);
    }
    /* Set pin to output */
    gpio_reg_write_mask(gpio_ctx->bank[bank_id].vaddr, IMX_GPIO_GDIR, pin_mask, pin_mask);

    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/resmgr/imx8/hw.c $ $Rev: 971888 $")
#endif
