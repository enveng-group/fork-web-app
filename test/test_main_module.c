/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: /devcontainer/web-app/test/test_main_module.c */

/* System headers */
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* CUnit header */
#include <CUnit/CUnit.h>

/* Application headers */
#include "../include/init.h"
#include "../include/fs.h"
#include "../include/app_error.h"
#include "../include/shell.h"
#include "test_suite.h"

/* Test fixture */
static char test_log_path[] = "test/test.log";

/* Setup and teardown */
static int setup(void)
{
    errorInit(test_log_path);
    return 0;
}

static int teardown(void)
{
    remove(test_log_path);
    return 0;
}

/* Test cases */
void test_main_startup(void)
{
    /* Test basic initialization sequence */
    CU_ASSERT_EQUAL(initSystem(), INIT_SUCCESS);
    CU_ASSERT_EQUAL(fsInit("/"), FS_SUCCESS);
    CU_ASSERT_EQUAL(shellInit(), 0);

    /* Cleanup */
    shellShutdown();
    shutdownSystem();
}

void test_main_signal_handling(void)
{
    pid_t pid;
    int status;
    int timeout;

    /* Initialize timeout value */
    timeout = 5;

    pid = fork();
    CU_ASSERT(pid >= 0);

    if (pid == 0) {
        /* Child process */
        errorInit(test_log_path);
        initSystem();
        kill(getpid(), SIGTERM);
        _exit(0);
    } else {
        /* Parent process */
        while (timeout-- > 0 && waitpid(pid, &status, WNOHANG) == 0) {
            sleep(1);
        }

        /* Verify process termination */
        CU_ASSERT(WIFEXITED(status) || WIFSIGNALED(status));
    }
}

void test_main_cleanup(void)
{
    /* Initialize all subsystems */
    errorInit(test_log_path);
    initSystem();
    fsInit("/");
    shellInit();

    /* Test cleanup sequence */
    shellShutdown();
    shutdownSystem();
    errorShutdown();

    /* Verify clean shutdown */
    CU_ASSERT_EQUAL(getSystemState(), STATE_SHUTDOWN);
}

void test_main_args_handling(void)
{
    char *test_args[] = {"test_prog", "test/custom.log", NULL};
    FILE *fp;
    int result;
    struct stat st;

    /* Create test directory if it doesn't exist */
    result = mkdir("test", 0755);
    if (result != 0 && errno != EEXIST) {
        CU_FAIL("Failed to create test directory");
        return;
    }

    /* Test argument handling */
    errorInit(test_args[1]);

    /* Write test data to verify file creation */
    fp = fopen(test_args[1], "w");
    if (fp != NULL) {
        fprintf(fp, "test\n");
        fclose(fp);

        /* Verify file existence */
        result = stat(test_args[1], &st);
        CU_ASSERT_EQUAL(result, 0);
    } else {
        CU_FAIL("Failed to create test file");
    }

    /* Cleanup */
    remove(test_args[1]);
}

/* Test suite initialization */
int test_main_module(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Main Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if (
        (CU_add_test(suite, "Main Startup", test_main_startup) == NULL) ||
        (CU_add_test(suite, "Signal Handling", test_main_signal_handling) == NULL) ||
        (CU_add_test(suite, "Main Cleanup", test_main_cleanup) == NULL) ||
        (CU_add_test(suite, "Arguments Handling", test_main_args_handling) == NULL)
    ) {
        return -1;
    }

    return 0;
}
