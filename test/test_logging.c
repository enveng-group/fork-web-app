/* System headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <glob.h>

/* CUnit headers */
#include <CUnit/CUnit.h>

/* Local headers */
#include "../include/common.h"
#include "../include/constants.h"
#include "../include/app_error.h"
#include "../include/logging.h"
#include "test_suite.h"

#define TEST_LOG_PATH "test.log"
#define TEST_LOG_MSG "Test log message"
#define MAX_ITERATIONS 1000

/* Test fixture setup and teardown */
static int
setup(void)
{
    return 0;
}

static int
teardown(void)
{
    remove(TEST_LOG_PATH);
    return 0;
}

/* Test cases */
void
test_log_init_null(void)
{
    CU_ASSERT_EQUAL(logInit(NULL, LOG_FLAG_NONE), -1);
}

void
test_log_init_valid(void)
{
    CU_ASSERT_EQUAL(logInit(TEST_LOG_PATH, LOG_FLAG_NONE), 0);
    logCleanup();
}

void
test_log_write(void)
{
    struct stat st;
    int result;

    logInit(TEST_LOG_PATH, LOG_FLAG_TIMESTAMP | LOG_FLAG_LEVEL);
    logWrite(LOG_INFO, TEST_LOG_MSG);

    result = stat(TEST_LOG_PATH, &st);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT(st.st_size > 0);

    logCleanup();
}

void
test_log_rotate(void)
{
    char old_path[MAX_LINE_LENGTH];
    struct stat st;
    glob_t globbuf;
    size_t i;
    int result;
    int glob_result;

    /* Initialize logging */
    result = logInit(TEST_LOG_PATH, LOG_FLAG_TIMESTAMP);
    CU_ASSERT_EQUAL(result, 0);

    /* Write enough data to trigger rotation */
    i = 0;
    while (i < MAX_ITERATIONS) {
        logWrite(LOG_INFO, TEST_LOG_MSG);
        i++;
    }

    /* Force rotation */
    logRotate();

    /* Verify new log file exists */
    result = stat(TEST_LOG_PATH, &st);
    CU_ASSERT_EQUAL(result, 0);

    /* Check for backup file (should exist with timestamp) */
    snprintf(old_path, sizeof(old_path), "%s.*", TEST_LOG_PATH);

    memset(&globbuf, 0, sizeof(globbuf));
    glob_result = glob(old_path, 0, NULL, &globbuf);
    CU_ASSERT_EQUAL(glob_result, 0);
    CU_ASSERT(globbuf.gl_pathc > 0);

    /* Cleanup */
    globfree(&globbuf);
    logCleanup();
}

void
test_log_levels(void)
{
    int result;
    struct stat st;
    long initial_size, final_size;

    result = logInit(TEST_LOG_PATH, LOG_FLAG_LEVEL);
    CU_ASSERT_EQUAL(result, 0);

    /* Get initial file size */
    stat(TEST_LOG_PATH, &st);
    initial_size = st.st_size;

    /* Test various log levels */
    logWrite(LOG_DEBUG, "Debug message"); /* Should be logged */
    logWrite(LOG_INFO, "Info message"); /* Should be logged */
    logWrite(LOG_WARNING, "Warning message"); /* Should be logged */
    logWrite(LOG_ERR, "Error message"); /* Should be logged */

    /* Get final file size */
    stat(TEST_LOG_PATH, &st);
    final_size = st.st_size;

    /* Verify log growth */
    CU_ASSERT(final_size > initial_size);

    logCleanup();
}

void
test_log_cleanup(void)
{
    FILE *fp;

    logInit(TEST_LOG_PATH, LOG_FLAG_NONE);
    logWrite(LOG_INFO, TEST_LOG_MSG);
    logCleanup();

    /* Verify we can initialize again */
    CU_ASSERT_EQUAL(logInit(TEST_LOG_PATH, LOG_FLAG_NONE), 0);
    logCleanup();

    /* Try to open file after cleanup */
    fp = fopen(TEST_LOG_PATH, "r");
    CU_ASSERT_PTR_NOT_NULL(fp);
    if (fp != NULL) {
        fclose(fp);
    }
}

/* Test suite initialization */
int
test_logging(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Logging Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    /* Add test cases */
    if ((CU_add_test(suite, "Log Init Null", test_log_init_null) == NULL) ||
        (CU_add_test(suite, "Log Init Valid", test_log_init_valid) == NULL) ||
        (CU_add_test(suite, "Log Write", test_log_write) == NULL) ||
        (CU_add_test(suite, "Log Rotate", test_log_rotate) == NULL) ||
        (CU_add_test(suite, "Log Levels", test_log_levels) == NULL) ||
        (CU_add_test(suite, "Log Cleanup", test_log_cleanup) == NULL)) {
        return -1;
    }

    return 0;
}
