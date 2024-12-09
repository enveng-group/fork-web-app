/**
 * \file compat.h
 * \brief Header file for compatibility functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#ifndef COMPAT_H
#define COMPAT_H

#include "compat_types.h"

/**
 * \brief Converts a string to a 64-bit integer.
 *
 * \param str The string to convert.
 * \param base The base to use for conversion.
 * \param endptr Pointer to the end of the parsed string.
 * \return The converted 64-bit integer.
 */
extern long compatStrtol64_asm(const char *str, int base, char **endptr);

/**
 * \brief Adds two 64-bit unsigned integers.
 *
 * \param result The result of the addition.
 * \param a The first integer.
 * \param b The second integer.
 */
extern void add_custom_uint64(custom_uint64_t *result, const custom_uint64_t *a, const custom_uint64_t *b);

/**
 * \brief Divides two 64-bit unsigned integers.
 *
 * \param numer The numerator.
 * \param denom The denominator.
 * \return The quotient of the division.
 */
extern compat_div_long_t div_custom_uint64(custom_uint64_t numer, custom_uint64_t denom);

#endif /* COMPAT_H */
