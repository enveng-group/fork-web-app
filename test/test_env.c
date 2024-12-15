#include <CUnit/CUnit.h>
#include "../include/env.h"
#include "../include/constants.h"
#include "test_suite.h"
#include <stdlib.h>
#include <string.h>

/* Test fixture */
static const char *TEST_ENV_PATH = "test/test.env";

/* Setup and teardown */
static int setup(void)
{
    FILE *fp;

    fp = fopen(TEST_ENV_PATH, "w");
    if (fp == NULL) {
        return -1;
    }

    fprintf(fp, "TEST_VAR=test_value\n");
    fprintf(fp, "EMPTY_VAR=\n");
    fprintf(fp, "# Comment line\n");
    fprintf(fp, "SPACE_VAR=value with spaces\n");

    fclose(fp);
    return 0;
}

static int teardown(void)
{
    remove(TEST_ENV_PATH);
    return 0;
}

/* Test cases */
void test_env_init_null(void)
{
    CU_ASSERT_EQUAL(envInit(NULL), -1);
    CU_ASSERT_EQUAL(envGetStatus(), ENV_INVALID_PARAM);
}

void test_env_init_valid(void)
{
    CU_ASSERT_EQUAL(envInit(TEST_ENV_PATH), 0);
    CU_ASSERT_EQUAL(envGetStatus(), ENV_SUCCESS);
}

void test_env_get_valid(void)
{
    char value[MAX_ENV_VALUE];

    envInit(TEST_ENV_PATH);

    CU_ASSERT_EQUAL(envGet("TEST_VAR", value, sizeof(value)), 0);
    CU_ASSERT_STRING_EQUAL(value, "test_value");
    CU_ASSERT_EQUAL(envGetStatus(), ENV_SUCCESS);
}

void test_env_get_invalid(void)
{
    char value[MAX_ENV_VALUE];

    envInit(TEST_ENV_PATH);

    CU_ASSERT_EQUAL(envGet("NONEXISTENT_VAR", value, sizeof(value)), -1);
    CU_ASSERT_EQUAL(envGetStatus(), ENV_NOT_FOUND);
}

void test_env_set_valid(void)
{
    char value[MAX_ENV_VALUE];

    envInit(TEST_ENV_PATH);

    CU_ASSERT_EQUAL(envSet("NEW_VAR", "new_value"), 0);
    CU_ASSERT_EQUAL(envGet("NEW_VAR", value, sizeof(value)), 0);
    CU_ASSERT_STRING_EQUAL(value, "new_value");
    CU_ASSERT_EQUAL(envGetStatus(), ENV_SUCCESS);
}

void test_env_set_invalid(void)
{
    CU_ASSERT_EQUAL(envSet(NULL, "value"), -1);
    CU_ASSERT_EQUAL(envGetStatus(), ENV_INVALID_PARAM);

    CU_ASSERT_EQUAL(envSet("VAR", NULL), -1);
    CU_ASSERT_EQUAL(envGetStatus(), ENV_INVALID_PARAM);
}

void test_env_cleanup(void)
{
    char value[MAX_ENV_VALUE];

    envInit(TEST_ENV_PATH);
    envSet("TEST_VAR", "test_value");
    envCleanup();

    CU_ASSERT_EQUAL(envGet("TEST_VAR", value, sizeof(value)), -1);
    CU_ASSERT_EQUAL(envGetStatus(), ENV_NOT_FOUND);
}

/* Test suite initialization */
int test_env(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Environment Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if (
        (CU_add_test(suite, "Env Init with NULL", test_env_init_null) == NULL) ||
        (CU_add_test(suite, "Env Init with Valid Path", test_env_init_valid) == NULL) ||
        (CU_add_test(suite, "Env Get Valid Variable", test_env_get_valid) == NULL) ||
        (CU_add_test(suite, "Env Get Invalid Variable", test_env_get_invalid) == NULL) ||
        (CU_add_test(suite, "Env Set Valid Variable", test_env_set_valid) == NULL) ||
        (CU_add_test(suite, "Env Set Invalid Variable", test_env_set_invalid) == NULL) ||
        (CU_add_test(suite, "Env Cleanup", test_env_cleanup) == NULL)
    ) {
        return -1;
    }

    return 0;
}
