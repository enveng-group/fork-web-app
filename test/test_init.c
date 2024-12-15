/* filepath: test/test_init.c */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/init.h"
#include "../include/app_error.h"
#include "test_suite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/* Test fixture */
static const char *TEST_CONFIG = "/tmp/test_system.conf";
static const char *TEST_ROOT = "/tmp/test_root";

/* Setup and teardown */
static int setup(void)
{
    FILE *fp;

    /* Create test config file */
    fp = fopen(TEST_CONFIG, "w");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "TEST_VAR=value\n");
    fclose(fp);

    /* Create test root directory */
    if (mkdir(TEST_ROOT, 0755) != 0 && errno != EEXIST) {
        return -1;
    }

    return 0;
}

static int teardown(void)
{
    remove(TEST_CONFIG);
    rmdir(TEST_ROOT);
    return 0;
}

/* Test cases */
void test_system_initialization(void)
{
    CU_ASSERT_EQUAL(initSystem(), INIT_SUCCESS);
    CU_ASSERT_EQUAL(getSystemState(), STATE_RUNNING);
    shutdownSystem();
}

void test_config_loading(void)
{
    CU_ASSERT_EQUAL(loadConfiguration(TEST_CONFIG), INIT_SUCCESS);
    CU_ASSERT_EQUAL(loadConfiguration("/nonexistent/path"), INIT_ERROR);
    CU_ASSERT_EQUAL(loadConfiguration(NULL), INIT_ERROR);
}

void test_system_shutdown(void)
{
    initSystem();
    CU_ASSERT_EQUAL(shutdownSystem(), INIT_SUCCESS);
    CU_ASSERT_EQUAL(getSystemState(), STATE_SHUTDOWN);
}

/* Test suite initialization */
int test_init(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Init Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if (
        (CU_add_test(suite, "System Initialization", test_system_initialization) == NULL) ||
        (CU_add_test(suite, "Configuration Loading", test_config_loading) == NULL) ||
        (CU_add_test(suite, "System Shutdown", test_system_shutdown) == NULL)
    ) {
        return -1;
    }

    return 0;
}
