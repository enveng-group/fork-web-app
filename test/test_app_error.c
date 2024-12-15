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
    errorInit(NULL); /* Use stderr for testing */
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
    errorInit(NULL);
    CU_ASSERT_PTR_NOT_NULL(stderr);
    errorShutdown();
}

void
test_app_error_logging(void)
{
    const char *test_msg = "Test error message";
    errorInit(NULL);
    errorLog(ERROR_INFO, test_msg);
    CU_ASSERT_STRING_EQUAL(errorGetLast(), test_msg);
    errorShutdown();
}

void
test_app_error_levels(void)
{
    errorInit(NULL);
    errorLog(ERROR_INFO, "Info message");
    errorLog(ERROR_WARNING, "Warning message");
    errorLog(ERROR_CRITICAL, "Critical message");
    CU_ASSERT_STRING_EQUAL(errorGetLast(), "Critical message");
    errorShutdown();
}

/* Test suite registration function */
int
test_app_error(void)
{
    CU_Suite *suite;

    suite = CU_add_suite("app_error", test_app_error_init, test_app_error_cleanup);
    if (suite == NULL) {
        return -1;
    }

    if (CU_add_test(suite, "Test error init with NULL", test_app_error_init_null) == NULL ||
        CU_add_test(suite, "Test error logging", test_app_error_logging) == NULL ||
        CU_add_test(suite, "Test error levels", test_app_error_levels) == NULL) {
        return -1;
    }

    return 0;
}
