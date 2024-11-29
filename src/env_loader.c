/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/env_loader.h"
#include "../include/validator.h"
#include "../include/logger.h"
#include "../include/error_handler.h"
#include "../include/config_loader.h"

#define MAX_LINE_LENGTH 256

int load_env_config(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        handle_error("Failed to open configuration file");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    config_t config = {0};

    while (fgets(line, sizeof(line), file))
    {
        char *key   = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (key && value)
        {
            if (strcmp(key, "app_name") == 0)
            {
                config.app_name = strdup(value);
            }
            else if (strcmp(key, "version") == 0)
            {
                config.version = atof(value);
            }
            else if (strcmp(key, "log_level") == 0)
            {
                config.log_level = strdup(value);
            }
            else if (strcmp(key, "enable_feature_x") == 0)
            {
                config.enable_feature_x = atoi(value);
            }
            else if (strcmp(key, "enable_feature_y") == 0)
            {
                config.enable_feature_y = atoi(value);
            }
            else if (strcmp(key, "enable_feature_z") == 0)
            {
                config.enable_feature_z = atoi(value);
            }
            else if (strcmp(key, "max_retries") == 0)
            {
                config.max_retries = atoi(value);
            }
            else if (strcmp(key, "app_mode") == 0)
            {
                config.app_mode = strdup(value);
            }
        }
    }

    if (fclose(file) != 0)
    {
        handle_error("Failed to close configuration file");
        free(config.app_name);
        free(config.log_level);
        free(config.app_mode);
        return -1;
    }

    // Free allocated memory
    free(config.app_name);
    free(config.log_level);
    free(config.app_mode);

    return 0;
}
