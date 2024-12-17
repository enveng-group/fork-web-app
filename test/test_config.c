/* filepath: /devcontainer/web-app/test/test_config.c */
#include <CUnit/CUnit.h>
#include "../include/config.h"
#include "test_suite.h"
#include <stdlib.h>
#include <string.h>

/* Test fixture */
static const char *TEST_CONFIG_PATH = "test/test_config.txt";

/* Setup and teardown */
static int setup(void)
{
    FILE *fp;

    fp = fopen(TEST_CONFIG_PATH, "w");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "key1=value1\n");
    fprintf(fp, "key2=value2\n");
    fclose(fp);

    return configInit(TEST_CONFIG_PATH);
}

static int teardown(void)
{
    configCleanup();
    return remove(TEST_CONFIG_PATH);
}

/* Non-static test functions to match test_suite.h */
void test_config_get_valid(void)
{
    char value[MAX_ENV_VALUE];
    int result;

    result = configGet("key1", value, sizeof(value));
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_STRING_EQUAL(value, "value1");
}

void test_config_get_invalid(void)
{
    char value[MAX_ENV_VALUE];
    int result;

    result = configGet("nonexistent", value, sizeof(value));
    CU_ASSERT_EQUAL(result, -1);
}

void test_config_cleanup(void)
{
    char value[MAX_ENV_VALUE];
    int result;

    configCleanup();
    result = configGet("key1", value, sizeof(value));
    CU_ASSERT_EQUAL(result, -1);
}

/* Test suite initialization */
int test_config(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Config Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if (CU_add_test(suite, "Config Get Valid", test_config_get_valid) == NULL ||
        CU_add_test(suite, "Config Get Invalid", test_config_get_invalid) == NULL ||
        CU_add_test(suite, "Config Cleanup", test_config_cleanup) == NULL) {
        return -1;
    }

    return 0;
}
