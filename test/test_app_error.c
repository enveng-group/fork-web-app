#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>
#include "../include/app_error.h"
#include "test_suite.h"

/* Test fixture setup and teardown */
static int
test_app_error_init(void)
{
    /* Initialize with stderr for testing */
    errorInit(NULL);
    return 0;
}

static int
test_app_error_cleanup(void)
{
    errorShutdown();
    return 0;
}

/* Test cases */
void
test_app_error_init_null(void)
{
    const char *last_error;

    /* Test initialization with NULL */
    errorInit(NULL);

    /* Verify stderr is being used */
    last_error = errorGetLast();
    CU_ASSERT_PTR_NOT_NULL(last_error);

    errorShutdown();
}

void
test_app_error_logging(void)
{
    const char *test_msg = "Test error message";
    const char *last_error;

    /* Initialize error handling */
    errorInit(NULL);

    /* Test logging a message */
    errorLog(ERROR_INFO, test_msg);
    last_error = errorGetLast();

    /* Verify the message was logged */
    CU_ASSERT_PTR_NOT_NULL(last_error);
    CU_ASSERT_STRING_EQUAL(last_error, test_msg);

    errorShutdown();
}

void
test_app_error_levels(void)
{
    const char *info_msg = "Info message";
    const char *warn_msg = "Warning message";
    const char *crit_msg = "Critical message";
    const char *last_error;

    /* Initialize error handling */
    errorInit(NULL);

    /* Test different error levels */
    errorLog(ERROR_INFO, info_msg);
    last_error = errorGetLast();
    CU_ASSERT_STRING_EQUAL(last_error, info_msg);

    errorLog(ERROR_WARNING, warn_msg);
    last_error = errorGetLast();
    CU_ASSERT_STRING_EQUAL(last_error, warn_msg);

    errorLog(ERROR_CRITICAL, crit_msg);
    last_error = errorGetLast();
    CU_ASSERT_STRING_EQUAL(last_error, crit_msg);

    /* Test invalid error level */
    errorLog(99, "Invalid level");
    last_error = errorGetLast();
    CU_ASSERT_STRING_EQUAL(last_error, crit_msg);

    errorShutdown();
}

/* Test suite registration */
int
test_app_error(void)
{
    CU_Suite *suite;

    suite = CU_add_suite("app_error", test_app_error_init, test_app_error_cleanup);
    if (suite == NULL) {
        return -1;
    }

    /* Add tests to suite */
    if ((CU_add_test(suite, "Test error init with NULL", test_app_error_init_null) == NULL) ||
        (CU_add_test(suite, "Test error logging", test_app_error_logging) == NULL) ||
        (CU_add_test(suite, "Test error levels", test_app_error_levels) == NULL)) {
        return -1;
    }

    return 0;
}
