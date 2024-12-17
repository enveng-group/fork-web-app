#include <stdio.h>
#include <sys/stat.h>  /* For mkdir() */
#include <sys/types.h> /* For mode_t */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../include/shell.h"
#include "test_suite.h"

/* Test fixtures */
#define TEST_DIR_PATH "test/shell_test"
#define TEST_FILE_NAME "test.txt"
#define TEST_CMD "ls test.txt"
#define TEST_BUILTIN "cd /tmp"

/* Global test state */
static char original_dir[FILENAME_MAX];

/* Setup function with proper error handling */
static int
test_shell_setup(void)
{
    FILE *fp = NULL;

    /* Store current directory */
    if (getcwd(original_dir, sizeof(original_dir)) == NULL) {
        return -1;
    }

    /* Create test directory with error checking */
    if (mkdir(TEST_DIR_PATH, 0755) != 0) {
        if (errno != EEXIST) {
            return -1;
        }
    }

    /* Change to test directory */
    if (chdir(TEST_DIR_PATH) != 0) {
        return -1;
    }

    /* Create test file */
    fp = fopen(TEST_FILE_NAME, "w");
    if (fp == NULL) {
        chdir(original_dir);
        return -1;
    }

    if (fclose(fp) != 0) {
        chdir(original_dir);
        return -1;
    }

    /* Initialize shell subsystem */
    if (shellInit() != 0) {
        unlink(TEST_FILE_NAME);
        chdir(original_dir);
        return -1;
    }

    return 0;
}

/* Teardown function with proper error handling */
static int
test_shell_teardown(void)
{
    int status = 0;
    int saved_errno;

    /* Shutdown shell subsystem */
    shellShutdown();

    /* Save errno in case of chdir failure */
    saved_errno = errno;

    /* Return to original directory first */
    if (chdir(original_dir) != 0) {
        fprintf(stderr, "Failed to return to original directory: %s\n",
                strerror(errno));
        status = -1;
        /* Continue cleanup despite error */
    }

    /* Restore errno */
    errno = saved_errno;

    /* Clean up test file */
    if (unlink(TEST_FILE_NAME) != 0) {
        if (errno != ENOENT) {
            fprintf(stderr, "Failed to remove test file: %s\n",
                    strerror(errno));
            status = -1;
        }
    }

    /* Clean up test directory */
    if (rmdir(TEST_DIR_PATH) != 0) {
        if (errno != ENOENT && errno != ENOTEMPTY) {
            fprintf(stderr, "Failed to remove test directory: %s\n",
                    strerror(errno));
            status = -1;
        }
    }

    return status;
}

/* Test cases - remove static keyword since they're declared in test_suite.h */
void
test_shell_execute_command(void)
{
    int status;

    /* Test empty command */
    status = shellExecuteCommand("");
    CU_ASSERT_EQUAL(status, CMD_SUCCESS);

    /* Test NULL command */
    status = shellExecuteCommand(NULL);
    CU_ASSERT_EQUAL(status, CMD_SUCCESS);

    /* Test valid external command */
    status = shellExecuteCommand(TEST_CMD);
    CU_ASSERT_EQUAL(status, CMD_SUCCESS);

    /* Test builtin command */
    status = shellExecuteCommand(TEST_BUILTIN);
    CU_ASSERT_EQUAL(status, CMD_SUCCESS);
}

void
test_shell_invalid_command(void)
{
    int status;

    status = shellExecuteCommand("nonexistentcommand");
    CU_ASSERT_NOT_EQUAL(status, CMD_SUCCESS);
}

/* Test suite initialization */
int
test_shell(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Shell Module Tests", test_shell_setup, test_shell_teardown);
    if (suite == NULL) {
        return -1;
    }

    if (CU_add_test(suite, "Shell Execute Command", test_shell_execute_command) == NULL ||
        CU_add_test(suite, "Shell Invalid Command", test_shell_invalid_command) == NULL) {
        return -1;
    }

    return 0;
}
