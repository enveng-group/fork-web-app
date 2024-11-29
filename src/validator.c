/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/validator.h"
#include "../include/config_loader.h"

// Custom safe copy function
size_t safeCopy(char *dst, const char *src, size_t size)
{
    size_t src_len = strlen(src);
    if (size > 0)
    {
        size_t copy_len = (src_len >= size) ? size - 1 : src_len;
        for (size_t i = 0; i < copy_len; ++i)
        {
            dst[i] = src[i];
        }
        dst[copy_len] = '\0';
    }
    return src_len;
}

/**
 * Function: validateConfig
 * -------------------------
 * Validates the configuration.
 *
 * config: Configuration structure to validate
 * value: Value to validate
 *
 * returns: 0 if validation is successful, otherwise an error code
 */
int validateConfig(config_t *config, const char *value)
{
    if (config == NULL || value == NULL)
    {
        return -1; // Error: Null pointer
    }

    if (strlen(value) >= sizeof(config->app_name))
    {
        // Handle the truncation error
        return ERANGE; // or another appropriate error code
    }

    safeCopy(config->app_name, value, sizeof(config->app_name));

    return 0; // Success
}
