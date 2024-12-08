/**
 * \file test_data_structures.c
 * \brief Unit tests for the data structures module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"
#include <stdlib.h>  /* Include stdlib.h for malloc and free */
#include "../include/data_structures.h"

/**
 * \brief Test case for data structure creation.
 */
void
test_data_structure_creation (void)
{
    struct DataStructure ds;
    ds.size = 0;
    ds.capacity = 10;
    ds.data = malloc (ds.capacity * sizeof (int));
    TEST_ASSERT (ds.data != NULL);
    TEST_ASSERT (ds.size == 0);
    free (ds.data);
}

/**
 * \brief Test case for data structure insertion.
 */
void
test_data_structure_insertion (void)
{
    struct DataStructure ds;
    ds.size = 0;
    ds.capacity = 10;
    ds.data = malloc (ds.capacity * sizeof (int));
    ds.data[ds.size++] = 42;
    TEST_ASSERT (ds.size == 1);
    TEST_ASSERT (ds.data[0] == 42);
    free (ds.data);
}

/**
 * \brief Main function to run the data structures test cases.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    RUN_TEST (test_data_structure_creation);
    RUN_TEST (test_data_structure_insertion);
    return 0;
}
