/**
 * \file test_utils.c
 * \brief Unit tests for the utils module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"

/**
 * \brief Test case for util_function_1.
 */
void
test_util_function_1 (void)
{
    int result = util_function_1 ();
    TEST_ASSERT_EQUAL (expected_value, result);
}

/**
 * \brief Test case for util_function_2.
 */
void
test_util_function_2 (void)
{
    int result = util_function_2 ();
    TEST_ASSERT_TRUE (result);
}

/**
 * \brief Main function to run all test cases.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    RUN_TEST (test_util_function_1);
    RUN_TEST (test_util_function_2);
    return 0;
}
