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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <devctl.h>

#include <hw/dcmd_gpio_imx.h>


/**
 * @brief GPIO resource manager pathname
 */
static char *gpio_resmgr_path = "/dev/gpio";

/**
 * @brief GPIO pin configurations set by command line options
 */
static unsigned int gpio_pin = UINT_MAX;
static unsigned int gpio_bank = UINT_MAX;
static unsigned int gpio_state = UINT_MAX;
static unsigned int gpio_write_output = 0;
static unsigned int gpio_set_output = 0;
static unsigned int gpio_set_input = 0;

/**
 * @brief Quiet mode enable flag
 *
 * When this flag is set to 1, all the application output messages are muted,
 * this includes the error messages as well.
 */
static unsigned int quiet_mode = 0;

/**
 * @brief Union of all the supported devctl messages
 */
static gpio_devctl_t gpio_msg;


/**
 * @brief Process the command line options
 *
 * @param argc
 * @param argv
 * @return int      0 on success, -1 otherwise
 */
static int parse_options(int argc, char *argv[])
{
    int option;
    char error_str[512] = {0};

    while ((option = getopt(argc, argv, "b:iN:o:p:qw:")) != -1) {

        switch (option) {
            case 'b':
                gpio_bank = (unsigned int)strtoul(optarg, &optarg, 0);
                break;
            case 'p':
                gpio_pin = (unsigned int)strtoul(optarg, &optarg, 0);
                break;
            case 'i':
                gpio_set_input = 1;
                break;
            case 'o':
                gpio_set_output = 1;
                gpio_state = (unsigned int)strtoul(optarg, &optarg, 0);
                break;
            case 'w':
                gpio_write_output = 1;
                gpio_state = (unsigned int)strtoul(optarg, &optarg, 0);
                break;
            case 'q':
                quiet_mode = 1;
                break;
            case 'N':
                gpio_resmgr_path = optarg;
                break;
            default:
                return -1;
        }
    }

    if (gpio_bank == UINT_MAX) {
        strlcat(error_str, "- Specify the GPIO bank number (-b)\n", sizeof(error_str));
    }
    if (gpio_pin == UINT_MAX) {
        strlcat(error_str, "- Specify the GPIO pin number (-p)\n", sizeof(error_str));
    }
    if ((gpio_set_output > 0) && (gpio_set_input > 0)) {
        strlcat(error_str, "- Specify 'set input' (-i) OR 'set output' (-o), not both\n", sizeof(error_str));
    }
    if ((gpio_write_output > 0) && (gpio_set_input > 0)) {
        strlcat(error_str, "- Specify 'set input' (-i) OR 'write output' (-w), not both\n", sizeof(error_str));
    }
    if ((gpio_write_output > 0) && (gpio_set_output > 0)) {
        strlcat(error_str, "- Specify 'set output' (-o) OR 'write output' (-w), not both\n", sizeof(error_str));
    }
    if (strlen(error_str) > 0) {
        if (quiet_mode == 0) fprintf(stderr, "Missing/invalid parameters:\n%sAbort.\n", error_str);
        return -1;
    }

    return 0;
}

/**
 * @brief Main routine of the GPIO test application
 *
 * Upon success the application returns the pin state (0 or 1). However,
 * the application will still return -1 on failures.
 *
 * @param argc
 * @param argv
 * @return int      GPIO state (0 or 1) on success, -1 on failures
 */
int main(int argc, char *argv[])
{
    int fd;
    int status;

    if (parse_options(argc, argv) != 0) {
        return -1;
    }

    fd = open(gpio_resmgr_path, O_RDWR);
    if (fd < 0) {
        if (quiet_mode == 0) fprintf(stderr, "Unable to open %s: %s\n", gpio_resmgr_path, strerror(errno));
        return -1;
    }

    /*
     * Set GPIO to input
     */
    if (gpio_set_input > 0)
    {
        memset(&gpio_msg, 0, sizeof(gpio_msg));
        gpio_msg.cmd_set_input.bank = (uint8_t)gpio_bank;
        gpio_msg.cmd_set_input.pin = (uint8_t)gpio_pin;

        status = devctl(fd, DCMD_GPIO_SET_INPUT, &gpio_msg.cmd_set_input, sizeof(gpio_msg.cmd_set_input), NULL);
        if (status) {
            if (quiet_mode == 0) fprintf(stderr, "Error: DCMD_GPIO_SET_INPUT returned %d\n", status);
            return -1;
        }
    }

    /*
     * Set GPIO to output
     */
    else if (gpio_set_output > 0)
    {
        memset(&gpio_msg, 0, sizeof(gpio_msg));
        gpio_msg.cmd_set_output.bank = (uint8_t)gpio_bank;
        gpio_msg.cmd_set_output.pin = (uint8_t)gpio_pin;
        gpio_msg.cmd_set_output.data = (uint8_t)gpio_state;

        /* Request to set the GPIO direction to output */
        status = devctl(fd, DCMD_GPIO_SET_OUTPUT, &gpio_msg.cmd_set_output, sizeof(gpio_msg.cmd_set_output), NULL);
        if (status) {
            if (quiet_mode == 0) fprintf(stderr, "Error: DCMD_GPIO_SET_OUTPUT returned %d\n", status);
            return -1;
        }
    }

    /*
     * Write GPIO output state
     */
    else if (gpio_write_output > 0)
    {
        memset(&gpio_msg, 0, sizeof(gpio_msg));
        gpio_msg.cmd_write.bank = (uint8_t)gpio_bank;
        gpio_msg.cmd_write.pin = (uint8_t)gpio_pin;
        gpio_msg.cmd_write.data = (uint8_t)gpio_state;

        /* Request to write the GPIO output state */
        status = devctl(fd, DCMD_GPIO_WRITE, &gpio_msg.cmd_write, sizeof(gpio_msg.cmd_write), NULL);
        if (status) {
            if (quiet_mode == 0) fprintf(stderr, "Error: DCMD_GPIO_WRITE returned %d\n", status);
            return -1;
        }
    }

    /*
     * Read GPIO state
     */
    memset(&gpio_msg, 0, sizeof(gpio_msg));
    gpio_msg.cmd_read.bank = (uint8_t)gpio_bank;
    gpio_msg.cmd_read.pin = (uint8_t)gpio_pin;

    /* Request to read the GPIO pin */
    status = devctl(fd, DCMD_GPIO_READ, &gpio_msg.cmd_read, sizeof(gpio_msg.cmd_read), NULL);
    if (status) {
        if (quiet_mode == 0) fprintf(stderr, "Error: DCMD_GPIO_READ returned %d\n", status);
        return -1;
    }

    /* Display the GPIO pin state */
    if (quiet_mode == 0) {
        printf("GPIO%d[%d]: %d\n", gpio_msg.cmd_read.bank,
            gpio_msg.cmd_read.pin, gpio_msg.cmd_read.data);
    }

    return (int)gpio_msg.cmd_read.data;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/test/main.c $ $Rev: 932877 $")
#endif
