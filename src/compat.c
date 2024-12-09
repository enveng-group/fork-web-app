/**
 * \file compat.c
 * \brief Implements compatibility functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/compat.h"
#include "../include/compat_types.h"
#include "../include/error_codes.h"
#include "../include/config_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_NULL(ptr) \
    if ((ptr) == NULL)  \
    {                   \
        return ERROR_NULL_POINTER; \
    }

/* Declare external assembly functions */
extern long compatStrtol64_asm(const char *str, int base, char **endptr);
extern void add_custom_uint64(custom_uint64_t *result, const custom_uint64_t *a, const custom_uint64_t *b);
extern compat_div_long_t div_custom_uint64(custom_uint64_t numer, custom_uint64_t denom);

/* Function definitions */
long convertStringToLong(const char *str, char **endptr, int base, int is_unsigned)
{
    if (is_unsigned)
    {
        return strtoul(str, endptr, base);
    }
    else
    {
        return strtol(str, endptr, base);
    }
}

long compatAtol(const char *str)
{
    return convertStringToLong(str, NULL, 10, 0);
}

long compatStrtol(const char *str, char **endptr, int base)
{
    return convertStringToLong(str, endptr, base, 0);
}

/**
 * \brief Converts a string to a 64-bit integer.
 *
 * \param str The string to convert.
 * \param endptr Pointer to the end of the parsed string.
 * \param base The base to use for conversion.
 * \return The converted 64-bit integer.
 */
unsigned long compatStrtoul(const char *str, char **endptr, int base)
{
    return convertStringToLong(str, endptr, base, 1);
}

long compatLabs(long n)
{
    return (n < 0) ? -n : n;
}

compat_div_long_t compatDiv(long numer, long denom)
{
    compat_div_long_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}

custom_long_t compatAtol64(const char *str)
{
    return (custom_long_t)convertStringToLong(str, NULL, 10, 0);
}

custom_uint64_t compatAtou64(const char *str)
{
    custom_uint64_t result;
    result.value = strtoul(str, NULL, 10);
    return result;
}
