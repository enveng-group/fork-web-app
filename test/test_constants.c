/* filepath: /devcontainer/web-app/test/test_constants.c */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/constants.h"
#include "../include/env.h"
#include "test_suite.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

/* Test fixture */
static const char *TEST_ENV_PATH = "test/test.env";

/* Setup and teardown */
static int
setup(void)
{
    FILE *fp;

    /* Create test directory if it doesn't exist */
    if (mkdir("test", 0755) != 0 && errno != EEXIST) {
        return -1;
    }

    /* Create environment file */
    fp = fopen(TEST_ENV_PATH, "w");
    if (fp == NULL) {
        return -1;
    }

    /* Write test environment variables with ALL required values */
    fprintf(fp, "APP_NAME=TestApp\n");
    fprintf(fp, "APP_VERSION=1.0.0\n");
    fprintf(fp, "APP_ENV=test\n");
    fprintf(fp, "APP_DEBUG=1\n");
    fprintf(fp, "APP_PORT=8080\n");
    fprintf(fp, "APP_HOST=localhost\n");
    fprintf(fp, "LOG_LEVEL=debug\n");
    fprintf(fp, "LOG_PATH=/var/log\n");
    fprintf(fp, "LOG_FORMAT=json\n");
    fprintf(fp, "LOG_MAX_SIZE=1048576\n");
    /* Add missing database variables */
    fprintf(fp, "DB_HOST=localhost\n");
    fprintf(fp, "DB_PORT=5432\n");
    fprintf(fp, "DB_NAME=webapp\n");
    fprintf(fp, "DB_USER=postgres\n");
    fprintf(fp, "DB_PASSWORD=\n");
    /* Add missing cache variables */
    fprintf(fp, "CACHE_DRIVER=file\n");
    fprintf(fp, "CACHE_PREFIX=webapp_cache_\n");
    fprintf(fp, "CACHE_TTL=3600\n");

    fclose(fp);
    return envInit(TEST_ENV_PATH);
}

static int
teardown(void)
{
    return remove(TEST_ENV_PATH);
}

/* Test cases */
void
test_constants_init(void)
{
    int status = constants_init();
    CU_ASSERT_EQUAL(status, EXIT_SUCCESS);
}

void
test_constants_app_values(void)
{
    CU_ASSERT_STRING_EQUAL(get_app_name(), "TestApp");
    CU_ASSERT_STRING_EQUAL(get_app_version(), "1.0.0");
    CU_ASSERT_STRING_EQUAL(get_app_env(), "test");
    CU_ASSERT_EQUAL(get_app_debug(), 1);
    CU_ASSERT_EQUAL(get_app_port(), 8080);
    CU_ASSERT_STRING_EQUAL(get_app_host(), "localhost");
}

void
test_constants_log_values(void)
{
    CU_ASSERT_STRING_EQUAL(get_log_level(), "debug");
    CU_ASSERT_STRING_EQUAL(get_log_path(), "/var/log");
    CU_ASSERT_STRING_EQUAL(get_log_format(), "json");
    CU_ASSERT_EQUAL(get_log_max_size(), 1048576);
}

/* Test suite registration */
int
test_constants(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Constants Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if ((CU_add_test(suite, "Constants Initialization", test_constants_init) == NULL) ||
        (CU_add_test(suite, "Application Values", test_constants_app_values) == NULL) ||
        (CU_add_test(suite, "Logging Values", test_constants_log_values) == NULL)) {
        return -1;
    }

    return 0;
}

#ifdef TEST_STANDALONE
int
main(void)
{
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    if (test_constants() != 0) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
#endif
