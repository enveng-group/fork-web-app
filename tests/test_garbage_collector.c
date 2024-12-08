/**
 * \file test_garbage_collector.c
 * \brief Unit tests for the garbage collector module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/test_framework.h"

/**
 * \brief Test case for the garbage collector.
 */
void
test_garbage_collector (void)
{
    /* Initialize the garbage collector */
    gc_init ();

    /* Test allocation */
    void *ptr = gc_alloc (100);
    test_assert (ptr != NULL);

    /* Test deallocation */
    gc_free (ptr);
    test_assert (gc_is_freed (ptr) == 1);

    /* Test double free */
    gc_free (ptr);
    test_assert (gc_is_freed (ptr) == 1);

    /* Finalize the garbage collector */
    gc_finalize ();
}

/**
 * \brief Main function to run the garbage collector test case.
 *
 * \return 0 on success, otherwise an error code.
 */
int
main (void)
{
    test_run (test_garbage_collector);
    return 0;
}
