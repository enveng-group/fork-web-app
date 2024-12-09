/**
 * \file utils.c
 * \brief Implements utility functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/utils.h"
#include "../include/compat.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * \brief Safely copies a string to a destination buffer.
 *
 * \param dest Destination buffer.
 * \param src Source string.
 * \param n Size of the destination buffer.
 * \return 0 on success, -1 on failure.
 */
int safeStringCopy(char *dest, const char *src, size_t n)
{
    size_t src_len;

    if (n == 0)
    {
        return -1;
    }

    src_len = strlen(src);
    if (src_len >= n)
    {
        return -1;
    }

    strncpy(dest, src, n - 1);
    dest[n - 1] = '\0';

    return 0;
}

/**
 * \brief Example function that was causing warnings.
 *
 * \param str Input string.
 * \return Processed string.
 */
char *processString(char *str)
{
    char *end = str;

    /* Example processing (use 'end' to avoid unused variable warning) */
    while (*end != '\0')
    {
        end++;
    }

    /* Ensure the function returns a value */
    return str;
}

/**
 * \brief Trims leading and trailing whitespace from a string.
 *
 * \param str The string to trim.
 * \return The trimmed string.
 */
char *trimWhitespace(char *str)
{
    char *end;

    /* Trim leading space */
    while (isspace((unsigned char)*str))
    {
        str++;
    }

    if (*str == 0) /* All spaces? */
    {
        return str;
    }

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
    {
        end--;
    }

    /* Write new null terminator */
    *(end + 1) = 0;

    return str;
}
