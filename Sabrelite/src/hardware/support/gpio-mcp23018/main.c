/*
 * Copyright (c) 2024, Boundary Devices
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>

#define IOMASK(x) (uint8_t)(1 << (x % 8))
#define IODIR(x) ((x > 7) ? 0x01 : 0x00)
#define GPPU(x)  ((x > 7) ? 0x0D : 0x0C)
#define GPIO(x)  ((x > 7) ? 0x13 : 0x12)
#define MAX_GPIOS 16
/**
 * @brief GPIO pin configurations set by command line options
 */
static int i2c_fd = -1;
static unsigned int i2c_addr = UINT_MAX;
static unsigned int i2c_bus = UINT_MAX;
static unsigned int gpio_pin = UINT_MAX;
static unsigned int gpio_state = UINT_MAX;
static unsigned int gpio_write_output = 0;
static unsigned int gpio_set_output = 0;
static unsigned int gpio_set_input = 0;

static int mcp23018_read(uint8_t reg, uint8_t *value)
{
    int rbytes;
    struct {
        i2c_send_t hdr;
        uint8_t    reg;
    } msgreg;
    struct {
        i2c_recv_t hdr;
        uint8_t    val;
    } msgval;

    msgreg.hdr.slave.addr = i2c_addr;
    msgreg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msgreg.hdr.len        = 1;
    msgreg.hdr.stop       = 1;
    msgreg.reg            = reg;
    if (devctl(i2c_fd, DCMD_I2C_SEND, &msgreg, sizeof(msgreg), NULL)) {
        fprintf(stderr, "%s(%x)1 failed!\n", __func__, reg);
        return (-1);
    }

    msgval.hdr.slave.addr = i2c_addr;
    msgval.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msgval.hdr.len        = 1;
    msgval.hdr.stop       = 1;
    if (devctl(i2c_fd, DCMD_I2C_RECV, &msgval, sizeof(msgval), &rbytes)) {
        fprintf(stderr, "%s(%x)2 failed!\n", __func__, reg);
        return (-1);
    }

    *value = msgval.val;

    return 0;
}

static int mcp23018_write(uint8_t reg, uint8_t value)
{
    struct {
        i2c_send_t hdr;
        uint8_t    reg;
        uint8_t    val;
    } msg;

    msg.hdr.slave.addr = i2c_addr;
    msg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
    msg.hdr.len        = 2;
    msg.hdr.stop       = 1;
    msg.reg            = reg;
    msg.val            = value;
    if (devctl (i2c_fd, DCMD_I2C_SEND, &msg, sizeof(msg), NULL)) {
        fprintf(stderr, "%s(%x, %x) failed!\n", __func__, reg, value);
        return (-1);
    }

    return 0;
}

static int mcp23018_update_bits(uint8_t reg, uint8_t value, uint8_t mask)
{
    uint8_t rd_reg;

    if (mcp23018_read(reg, &rd_reg) < 0) {
        fprintf(stderr, "%s(%x, %x) failed to read\n", __func__, reg, value);
        return (-1);
    }

    rd_reg &= (uint8_t)~mask;
    rd_reg |= (uint8_t)(mask & value);
    if (mcp23018_write(reg, rd_reg) < 0) {
        fprintf(stderr, "%s(%x, %x) failed to read\n", __func__, reg, value);
        return (-1);
    }

    return 0;
}

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

    while ((option = getopt(argc, argv, "a:b:i:o:p:w:")) != -1) {

        switch (option) {
            case 'a':
                i2c_addr = (unsigned int)strtoul(optarg, &optarg, 0);
                break;
            case 'b':
                i2c_bus = (unsigned int)strtoul(optarg, &optarg, 0);
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
            default:
                return -1;
        }
    }

    if ((i2c_addr == UINT_MAX) || (i2c_bus == UINT_MAX)) {
        strlcat(error_str, "- Specify the I2C bus and addr for the MCP23018\n", sizeof(error_str));
    } else if (gpio_pin == UINT_MAX) {
        strlcat(error_str, "- Specify the GPIO pin number (-p)\n", sizeof(error_str));
    } else if (gpio_pin >= MAX_GPIOS) {
        strlcat(error_str, "- GPIO pin must be 0-15\n", sizeof(error_str));
    } else if ((gpio_set_output > 0) && (gpio_set_input > 0)) {
        strlcat(error_str, "- Specify 'set input' (-i) OR 'set output' (-o), not both\n", sizeof(error_str));
    } else if ((gpio_write_output > 0) && (gpio_set_input > 0)) {
        strlcat(error_str, "- Specify 'set input' (-i) OR 'write output' (-w), not both\n", sizeof(error_str));
    } else if ((gpio_write_output > 0) && (gpio_set_output > 0)) {
        strlcat(error_str, "- Specify 'set output' (-o) OR 'write output' (-w), not both\n", sizeof(error_str));
    }
    if (strlen(error_str) > 0) {
        fprintf(stderr, "Missing/invalid parameters:\n%sAbort.\n", error_str);
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
    int status = 0;
    char i2c_port[20];

    if (parse_options(argc, argv) != 0) {
        return -1;
    }

    sprintf(i2c_port, "/dev/i2c%d", i2c_bus);
    if ((i2c_fd = open(i2c_port, O_RDWR)) < 0) {
        fprintf(stderr, "could not open %s - %s", i2c_port, strerror(errno));
        return (-1);
    }

    /* Set pull-up as open drain */
    status = mcp23018_update_bits(GPPU(gpio_pin), IOMASK(gpio_pin), IOMASK(gpio_pin));
    if (status < 0) {
        fprintf(stderr, "couldn't set pull-up %d\n", status);
        return (-1);
    }

    if (gpio_set_input > 0) {
        status = mcp23018_update_bits(IODIR(gpio_pin), IOMASK(gpio_pin), IOMASK(gpio_pin));
    } else if (gpio_set_output > 0) {
        status = mcp23018_update_bits(IODIR(gpio_pin), 0, IOMASK(gpio_pin));
    }
    if (status < 0) {
        fprintf(stderr, "couldn't set direction %d\n", status);
        return (-1);
    }

    if (gpio_write_output > 0) {
        status = mcp23018_update_bits(IODIR(gpio_pin), 0, IOMASK(gpio_pin));
        if (gpio_state)
            status |= mcp23018_update_bits(GPIO(gpio_pin), IOMASK(gpio_pin), IOMASK(gpio_pin));
        else
            status |= mcp23018_update_bits(GPIO(gpio_pin), 0, IOMASK(gpio_pin));
    } else {
        uint8_t reg;

        if (!(status = mcp23018_read(GPIO(gpio_pin), &reg)))
            fprintf(stdout, "GPIO[%d] = %d\n", gpio_pin, !!(reg & IOMASK(gpio_pin)));
    }
    if (status < 0) {
        fprintf(stderr, "couldn't set/get gpio %d\n", status);
        return (-1);
    }

    return 0;
}
