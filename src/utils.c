/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "../include/utils.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

size_t
utils_safeCopy (char *dst, const char *src, size_t size)
{
    size_t i;

    if (size == 0)
        {
            return 0;
        }

    i = 0;
    for (i = 0; i < size - 1 && src[i] != '\0'; i++)
        {
            dst[i] = src[i];
        }
    dst[i] = '\0';
    return i;
}

char *trimWhitespace(char *str) {
    char *end = str;

    /* Trim leading space */
    while (isspace((unsigned char)*str)) str++;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    /* Write new null terminator character */
    end[1] = '\0';

    return str;
}
