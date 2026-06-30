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

#include <signal.h>
#include <login.h>
#include <secpol/secpol.h>
#include <secpol/ids.h>

#include "proto.h"
#include "log.h"


/* Store User ID and Group ID - configurable via options */
static char *resmgr_uid_gid = NULL;
/* Store resource manager pathname - configurable via options */
static char *resmgr_path = GPIO_RESMGR_PATH;

/* Local flag to notify dispatcher loop to terminate */
static volatile unsigned done = 0;
/* Local pointer to the dispacher context for the exit handler */
static dispatch_context_t *local_ctp = NULL;


/**
 * @brief Handle signal that terminates the main dispatcher loop
 *
 * @param signo     Signal ID that triggered execution of the handler
 */
static void exit_handler(int __attribute__((unused)) signo)
{
    /* Set the flag to terminate the main loop */
    done = 1;
    if (local_ctp != NULL) {
        dispatch_unblock(local_ctp);
    }
}

/**
 * @brief Attach the exit handler to the specified signal
 *
 * @param signo     Signal ID that will trigger execution of the handler
 * @return int      errno
 */
static int register_signal_handler(int signo)
{
    int status;
    struct sigaction sa = {0};

    /* Assign the exit handler */
    sa.sa_handler = exit_handler;
    status = sigaction(signo, &sa, NULL);
    if (status < 0) {
        return errno;
    }
    return EOK;
}

/**
 * @brief Process command line options
 *
 * @param argc      Number of command line arguments
 * @param argv      Array of command line arguments
 * @return int      0 on success, -1 otherwise
 */
static int parse_options(int argc, char *argv[])
{
    int option;

    while ((option = getopt(argc, argv, "N:U:")) != -1) {

        switch (option) {
            case 'N':
                resmgr_path = optarg;
                break;
            case 'U':
                resmgr_uid_gid = optarg;
                break;
            default:
                return -1;
        }
    }
    return 0;
}

/**
 * @brief Main routine for the GPIO resource manager
 *
 * @param argc      Number of command line arguments
 * @param argv      Array of command line arguments
 * @return int      EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int main(int argc, char *argv[])
{
    int                     status;
    gpio_dev_t              dev;
    resmgr_connect_funcs_t  connect_funcs;
    resmgr_io_funcs_t       io_funcs;

    GPIO_SLOG_INFO("Starting GPIO resource manager...");
    memset(&dev, 0, sizeof(dev));

    /* Process program arguments */
    if (parse_options(argc, argv) != 0) {
        GPIO_SLOG_ERROR("Error while parsing options!");
        status = EXIT_FAILURE;
        goto fail0;
    }

    /* Create dispatcher */
    dev.dpp = dispatch_create();
    if (dev.dpp == NULL) {
        GPIO_SLOG_ERROR("Failed to create dispatcher!");
        status = EXIT_FAILURE;
        goto fail0;
    }

    /* Initialize the default IO function */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
                     _RESMGR_IO_NFUNCS, &io_funcs);

    /* Override IO functions with local definitions */
    io_funcs.devctl = io_devctl;
    io_funcs.lock = NULL;

    /* Set access permission */
    iofunc_attr_init(&dev.hdr, S_IFCHR | 0664, NULL, NULL);

    /* Attach to the process manager and publish IO functions */
    dev.id = resmgr_attach(dev.dpp, NULL, resmgr_path, _FTYPE_ANY, 0,
        &connect_funcs, &io_funcs, &dev);
    if (dev.id < 0) {
        GPIO_SLOG_ERROR("Failed to attach to pathname %s: %s", resmgr_path, strerror(errno));
        status = EXIT_FAILURE;
        goto fail0;
    }

    /* Get the dispatcher context */
    dev.ctp = dispatch_context_alloc(dev.dpp);
    if (dev.ctp == NULL) {
        GPIO_SLOG_ERROR("Failed to allocate dispatcher context: %s", strerror(errno));
        status = EXIT_FAILURE;
        goto fail0;
    }
    /* Save context for the exit handler */
    local_ctp = dev.ctp;

    /* Register the exit handler against SIGTERM (kill or slay) and SIGINT (Ctrl-C) */
    status = register_signal_handler(SIGTERM);
    if (status != EOK) {
        GPIO_SLOG_ERROR("Failed to register signal handler: %s", strerror(status));
        status = EXIT_FAILURE;
        goto fail0;
    }
    status = register_signal_handler(SIGINT);
    if (status != EOK) {
        GPIO_SLOG_ERROR("Failed to register signal handler: %s", strerror(status));
        status = EXIT_FAILURE;
        goto fail0;
    }

    /* Execute platform-specific initialization */
    if (hw_init(&dev) == NULL) {
        GPIO_SLOG_ERROR("Failed to initialize the GPIOs");
        status = EXIT_FAILURE;
        goto fail0;
    }

    /* Detach process as a daemon */
    status = procmgr_daemon(EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE);
    if (status < 0) {
        GPIO_SLOG_ERROR("Failed to detach process as a daemon: %s", strerror(errno));
        status = EXIT_FAILURE;
        goto fail1;
    }

    /* Switch process to the proper security type */
    secpol_transition_type(NULL, NULL, 0);

    /* Set the UID/GID specified via the command line */
    if (resmgr_uid_gid != NULL) {
        status = set_ids_from_arg(resmgr_uid_gid);
        if (status != 0) {
            GPIO_SLOG_ERROR("Failed to set the specified User/Group ID %s: %s", resmgr_uid_gid, strerror(errno));
            status = EXIT_FAILURE;
            goto fail1;
        }
    }

    /* Update to the final UID/GID in case option '-U' was used */
    dev.hdr.uid = getuid();
    dev.hdr.gid = getgid();

    /* Main loop: waiting for messages */
    while (done == 0) {
        if (dispatch_block(dev.ctp) != NULL) {
            dispatch_handler(dev.ctp);
        } else {
            GPIO_SLOG_INFO("Dispatcher stopped: %s", strerror(errno));
            done = 1;
        }
    }
    status = EXIT_SUCCESS;

 fail1:
    /* Release platform-specific resources */
    hw_fini(&dev);
 fail0:
    GPIO_SLOG_INFO("GPIO resource manager has terminated.");
    return status;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.1.0/trunk/hardware/support/gpio-imx/resmgr/main.c $ $Rev: 932877 $")
#endif
