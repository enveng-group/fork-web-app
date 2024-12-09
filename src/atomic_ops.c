/**
 * \file atomic_ops.c
 * \brief Implementation of atomic operations.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/atomic_ops.h"

/**
 * \brief Atomically increments an integer.
 *
 * \param value Pointer to the integer to increment.
 */
void atomicIncrement(int *value)
{
    __asm__ __volatile__ ("lock incl (%0)" : : "r"(value) : "memory");
}
