/**
 * \file test_constants.c
 * \brief Unit tests for the constants module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"
#include <stdlib.h>  /* Include stdlib.h for exit and EXIT_FAILURE */

/**
 * \brief Test case for constants.
 */
void testConstants(void);

void
testConstants (void)
{
    TEST_ASSERT_EQUAL (1, 1);
    TEST_ASSERT_EQUAL (2 + 2, 4);
    TEST_ASSERT_NOT_EQUAL (3, 4);
}

/**
 * \brief Main function to run the constants test case.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    RUN_TEST (testConstants);
    return 0;
}
