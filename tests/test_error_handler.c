/**
 * \file test_error_handler.c
 * \brief Unit tests for the error handler module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"

/**
 * \brief Test case for the error handler.
 */
void
test_error_handler (void)
{
    TEST_ASSERT (error_handler (NULL) == ERROR_NULL_INPUT);
    TEST_ASSERT (error_handler (INVALID_INPUT) == ERROR_INVALID_INPUT);
    TEST_ASSERT (error_handler (VALID_INPUT) == ERROR_NONE);
}

/**
 * \brief Main function to run the error handler test case.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    RUN_TEST (test_error_handler);
    return 0;
}
