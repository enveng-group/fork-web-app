/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "validator.h"
#include "config_loader.h"
#include "logger.h"
#include "utils.h"

// Remove safeCopy implementation

/**
 * Function: validateConfig
 * -------------------------
 * Validates the configuration.
 *
 * config: Configuration structure to validate
 * value: Value to validate
 */
int validateConfig(config_t *config, const char *value)
{
    char app_name_buffer[256];
    utils_safeCopy(app_name_buffer, value, sizeof(app_name_buffer));

    // Perform basic validation logic
    if (config == NULL || value == NULL)
    {
        logError("Validation failed: config or value is NULL");
        return -1; // Invalid parameters
    }

    if (strlen(config->app_name) == 0)
    {
        logError("Validation failed: app_name is empty");
        return -2; // Invalid app name
    }

    if (config->version <= 0)
    {
        logError("Validation failed: version is invalid");
        return -3; // Invalid version
    }

    if (strlen(config->document_root) == 0)
    {
        logError("Validation failed: document_root is empty");
        return -4; // Invalid document root
    }

    if (strlen(config->rec_file_path) == 0)
    {
        logError("Validation failed: rec_file_path is empty");
        return -5; // Invalid record file path
    }

    if (strlen(config->auth_file) == 0)
    {
        logError("Validation failed: auth_file is empty");
        return -6; // Invalid auth file
    }

    // Additional validation logic can be added here

    return 0; // Return appropriate error code or success
}
