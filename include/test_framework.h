/**
 * \file test_framework.h
 * \brief Header file for the test framework.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>

/**
 * \brief Asserts a condition in a test case.
 *
 * \param condition Condition to assert.
 */
#define TEST_ASSERT(condition)                                                \
    do                                                                        \
        {                                                                     \
            if (!(condition))                                                 \
                {                                                             \
                    fprintf (stderr,                                          \
                             "Assertion failed: %s, file %s, line %d\n",      \
                             #condition, __FILE__, __LINE__);                 \
                    return 1;                                                 \
                }                                                             \
        }                                                                     \
    while (0)

/**
 * \brief Asserts that two values are equal in a test case.
 *
 * \param expected Expected value.
 * \param actual Actual value.
 */
#define TEST_ASSERT_EQUAL(expected, actual) \
    if ((expected) != (actual)) { \
        printf("Test failed: %s:%d: expected %d, got %d\n", __FILE__, __LINE__, (expected), (actual)); \
        exit(EXIT_FAILURE); \
    }

/**
 * \brief Asserts that two values are not equal in a test case.
 *
 * \param expected Expected value.
 * \param actual Actual value.
 */
#define TEST_ASSERT_NOT_EQUAL(expected, actual) \
    if ((expected) == (actual)) { \
        printf("Test failed: %s:%d: expected not %d, got %d\n", __FILE__, __LINE__, (expected), (actual)); \
        exit(EXIT_FAILURE); \
    }

/**
 * \brief Defines a test case.
 *
 * \param name Name of the test case.
 */
#define TEST_CASE(name) int test_##name (void)

/**
 * \brief Runs a test case.
 *
 * \param test Name of the test case to run.
 */
#define RUN_TEST(test)                                                          \
    do                                                                          \
    {                                                                           \
        printf("Running %s...\n", #test);                                       \
        test();                                                                 \
        printf("%s passed.\n", #test);                                          \
    } while (0)

/**
 * \brief Defines a test suite.
 *
 * \param name Name of the test suite.
 */
#define TEST_SUITE(name) void run_##name (void)

#endif /* TEST_FRAMEWORK_H */
