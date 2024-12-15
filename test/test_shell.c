#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <string.h>
#include <unistd.h>
#include "../include/shell.h"
#include "test_suite.h"

/* Test fixtures */
static const char *TEST_CMD = "ls -l";
static const char *TEST_BUILTIN = "cd /tmp";

/* Setup and teardown */
static int setup(void)
{
    return shellInit();
}

static int teardown(void)
{
    shellShutdown();
    return 0;
}

/* Test cases */
void test_shell_execute_command(void)
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

void test_shell_invalid_command(void)
{
    int status;

    status = shellExecuteCommand("nonexistentcommand");
    CU_ASSERT_NOT_EQUAL(status, CMD_SUCCESS);
}

/* Test suite initialization */
int test_shell(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Shell Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if (
        (CU_add_test(suite, "Shell Execute Command", test_shell_execute_command) == NULL) ||
        (CU_add_test(suite, "Shell Invalid Command", test_shell_invalid_command) == NULL)
    ) {
        return -1;
    }

    return 0;
}
