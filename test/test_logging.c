/* System headers */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

/* CUnit headers */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

/* Local headers */
#include "../include/logging.h"
#include "test_suite.h"

/* Test fixture */
static const char *TEST_LOG_PATH = "test/test.log";

/* Setup and teardown */
static int setup(void)
{
    return 0;
}

static int teardown(void)
{
    return remove(TEST_LOG_PATH);
}

/* Test cases */
void test_log_init_null(void)
{
    CU_ASSERT_EQUAL(logInit(NULL), -1);
}

void test_log_init_valid(void)
{
    CU_ASSERT_EQUAL(logInit(TEST_LOG_PATH), 0);
    logCleanup();
}

void test_log_write(void)
{
    const char *test_message = "Test log message";
    struct stat st;

    logInit(TEST_LOG_PATH);
    logWrite(LOG_INFO, "%s", test_message);
    logCleanup();

    CU_ASSERT_EQUAL(stat(TEST_LOG_PATH, &st), 0);
    CU_ASSERT(st.st_size > 0);
}

void test_log_rotate(void)
{
    struct stat st;
    int result;
    FILE *fp;

    /* Test code */
    result = logInit(TEST_LOG_PATH);
    CU_ASSERT_EQUAL(result, 0);

    fp = fopen(TEST_LOG_PATH, "r");
    CU_ASSERT_PTR_NOT_NULL(fp);
    if (fp) {
        fclose(fp);
    }

    result = stat(TEST_LOG_PATH, &st);
    CU_ASSERT_EQUAL(result, 0);

    logCleanup();
}

void test_log_levels(void)
{
    struct stat st;
    int result;
    long size_before;
    long size_after;

    /* Initialize log system */
    result = logInit(TEST_LOG_PATH);
    CU_ASSERT_EQUAL(result, 0);

    /* Get initial file size */
    result = stat(TEST_LOG_PATH, &st);
    size_before = st.st_size;

    /* Test different log levels */
    logWrite(LOG_DEBUG, "Debug message");  /* Should not be logged */
    logWrite(LOG_INFO, "Info message");    /* Should be logged */
    logWrite(LOG_WARNING, "Warning message"); /* Should be logged */
    logWrite(LOG_ERROR, "Error message"); /* Should be logged */

    /* Get final file size */
    result = stat(TEST_LOG_PATH, &st);
    size_after = st.st_size;

    /* Verify size increased */
    CU_ASSERT_TRUE(size_after > size_before);

    logCleanup();
}

void test_log_cleanup(void)
{
    logInit(TEST_LOG_PATH);
    logWrite(LOG_INFO, "Test message");
    logCleanup();

    /* Verify we can initialize again after cleanup */
    CU_ASSERT_EQUAL(logInit(TEST_LOG_PATH), 0);
    logCleanup();
}

/* Test suite initialization */
int test_logging(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Logging Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if (
        (CU_add_test(suite, "Log Init with NULL", test_log_init_null) == NULL) ||
        (CU_add_test(suite, "Log Init with Valid Path", test_log_init_valid) == NULL) ||
        (CU_add_test(suite, "Log Write", test_log_write) == NULL) ||
        (CU_add_test(suite, "Log Rotate", test_log_rotate) == NULL) ||
        (CU_add_test(suite, "Log Levels", test_log_levels) == NULL) ||
        (CU_add_test(suite, "Log Cleanup", test_log_cleanup) == NULL)
    ) {
        return -1;
    }

    return 0;
}
