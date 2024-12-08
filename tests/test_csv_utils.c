#include "../include/csv_utils.h"
#include <assert.h>
#include <stdlib.h>  /* Include stdlib.h for exit and EXIT_FAILURE */

/**
 * \brief Test case for CSV reading functionality.
 */
void
testCsvRead (void)
{
    int expected_value = 0; /* Define expected_value */
    assert (csv_read ("test.csv") == expected_value);
}

/**
 * \brief Test case for CSV writing functionality.
 */
void
testCsvWrite (void)
{
    void *data = NULL; /* Define data */
    int success = 0;   /* Define success */
    assert (csv_write ("output.csv", data) == success);
}

/**
 * \brief Main function to run the CSV utils test cases.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    testCsvRead ();
    testCsvWrite ();
    return 0;
}
