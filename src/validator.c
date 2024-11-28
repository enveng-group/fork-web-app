/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/validator.h"
#include "../include/logger.h"
#include "../include/config_loader.h"

/**
 * Function: validate_config
 * -------------------------
 * Validates the configuration.
 *
 * config: Configuration structure to validate
 * value: Value to validate
 *
 * returns: 0 if validation is successful, otherwise an error code
 */
int validate_config(config_t *config, const char *value)
{
    if (config == NULL || value == NULL)
    {
        return -1; // Error: Null pointer
    }

    strncpy(config->app_name, value, sizeof(config->app_name) - 1);
    config->app_name[sizeof(config->app_name) - 1] = '\0';  // Ensure null termination
    return 0; // Success
}

int validate_integer(const char *value) {
    for (int i = 0; value[i] != '\0'; i++) {
        if (!isdigit(value[i])) {
            return -1; // Invalid integer
        }
    }
    return 0; // Valid integer
}

int validate_double(const char *value) {
    int dot_count = 0;
    for (int i = 0; value[i] != '\0'; i++) {
        if (!isdigit(value[i])) {
            if (value[i] == '.' && dot_count == 0) {
                dot_count++;
            } else {
                return -1; // Invalid double
            }
        }
    }
    return 0; // Valid double
}

int validate_string(const char *value) {
    if (strlen(value) == 0) {
        return -1; // Invalid string
    }
    return 0; // Valid string
}

int validate_boolean(const char *value) {
    if (strcmp(value, "true") != 0 && strcmp(value, "false") != 0) {
        return -1; // Invalid boolean
    }
    return 0; // Valid boolean
}
