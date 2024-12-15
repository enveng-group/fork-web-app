#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "test_suite.h"
#include "../include/init.h"
#include "../include/shell.h"

/* Function declarations */
int test_shell(void);
int test_init(void);
int test_app_error(void);
int test_config(void);
int test_constants(void);
int test_env(void);
int test_fs(void);
int test_logging(void);
int test_main_module(void);

int
main(void)
{
    int result = 0;

    /* Initialize CUnit test registry */
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    /* Add test suites */
    if (test_app_error() != 0 ||
        test_config() != 0 ||
        test_constants() != 0 ||
        test_env() != 0 ||
        test_fs() != 0 ||
        test_init() != 0 ||
        test_logging() != 0 ||
        test_shell() != 0 ||
        test_main_module() != 0) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run tests */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    result = CU_basic_run_tests();

    /* Clean up and return */
    CU_cleanup_registry();
    printf("All tests completed with status: %d\n", result);
    return CU_get_error();
}
