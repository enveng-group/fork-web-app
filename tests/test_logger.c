/**
 * \file test_logger.c
 * \brief Unit tests for the logger module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"

/**
 * \brief Test case for logger initialization.
 */
void
test_logger_initialization (void)
{
    /* Your initialization test code here */
    TEST_ASSERT (logger_init () == 0);
}

/**
 * \brief Test case for logger logging.
 */
void
test_logger_logging (void)
{
    /* Your logging test code here */
    logger_log ("Test message");
    TEST_ASSERT (logger_get_last_message () == "Test message");
}

/**
 * \brief Test case for logger error handling.
 */
void
test_logger_error_handling (void)
{
    /* Your error handling test code here */
    logger_log (NULL);
    TEST_ASSERT (logger_get_last_error () == ERROR_NULL_MESSAGE);
}

/**
 * \brief Main function to run all test cases.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    RUN_TEST (test_logger_initialization);
    RUN_TEST (test_logger_logging);
    RUN_TEST (test_logger_error_handling);
    return 0;
}
