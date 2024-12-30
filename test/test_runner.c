/* filepath: /home/appuser/fork-web-app/test/test_runner.c */
/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#include "test_suites.h"

/* Declarations of test suite initialization functions */
int init_web_server_suite(CU_pSuite suite);
int init_web_server_security_suite(CU_pSuite suite);

int
main(void)
{
    CU_pSuite web_server_suite;
    CU_pSuite web_server_security_suite;

    /* Initialize CUnit registry */
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    /* Create suites */
    web_server_suite = CU_add_suite("Web Server Tests", NULL, NULL);
    if (web_server_suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    web_server_security_suite = CU_add_suite("Web Server Security Tests", NULL, NULL);
    if (web_server_security_suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Initialize test suites */
    if (init_web_server_suite(web_server_suite) != 0 ||
        init_web_server_security_suite(web_server_security_suite) != 0) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run tests */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
