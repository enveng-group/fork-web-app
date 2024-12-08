/**
 * \file test_main.c
 * \brief Main test runner for all modules.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"
#include <stdio.h>

/**
 * \brief Runs all test cases.
 */
void run_tests (void);

int
main (void)
{
    run_tests ();
    return 0;
}

/**
 * \brief Runs all test cases.
 */
void
run_tests (void)
{
    test_config_loader ();
    test_constants ();
    test_data_structures ();
    test_env_loader ();
    test_error_handler ();
    test_garbage_collector ();
    test_logger ();
    test_utils ();
    test_validator ();
}
