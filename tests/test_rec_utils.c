/**
 * \file test_rec_utils.c
 * \brief Unit tests for the REC utilities module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"
#include "error_codes.h"
#include "rec_utils.h"
#include "records.h"

/**
 * \brief Test case for converting CSV to REC.
 */
void
test_csvToRec (void)
{
    int result = csvToRec ("data/test.csv", "data/test.rec");
    TEST_ASSERT_EQUAL (SUCCESS, result);
}

/**
 * \brief Test case for converting REC to CSV.
 */
void
test_recToCsv (void)
{
    int result = recToCsv ("data/test.rec", "data/test.csv");
    TEST_ASSERT_EQUAL (SUCCESS, result);
}

/**
 * \brief Test case for deleting a record from a REC file.
 */
void
test_deleteRec (void)
{
    int result = deleteRec ("data/test.rec", "record_id");
    TEST_ASSERT_EQUAL (SUCCESS, result);
}

/**
 * \brief Main function to run all test cases.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    RUN_TEST (test_csvToRec);
    RUN_TEST (test_recToCsv);
    RUN_TEST (test_deleteRec);
    return 0;
}
