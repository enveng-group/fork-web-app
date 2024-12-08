/**
 * \file test_env_loader.c
 * \brief Unit tests for the environment loader module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"
#include "../include/env_loader.h"

/**
 * \brief Test case for the environment loader.
 */
void
test_env_loader (void)
{
    TEST_ASSERT (load_environment ("valid_env") == 0);
    TEST_ASSERT (load_environment ("invalid_env") != 0);
}

/**
 * \brief Main function to run the environment loader test case.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    RUN_TEST (test_env_loader);
    return 0;
}
