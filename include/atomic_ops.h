/**
 * \file atomic_ops.h
 * \brief Header file for atomic operations.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef ATOMIC_OPS_H
#define ATOMIC_OPS_H

/**
 * \brief Atomically increments an integer.
 *
 * \param value Pointer to the integer to increment.
 */
void atomicIncrement (int *value);

#endif /* ATOMIC_OPS_H */
