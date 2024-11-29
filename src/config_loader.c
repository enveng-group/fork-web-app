/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/config_loader.h"
#include "../include/validator.h"
#include "../include/logger.h"
#include "../include/error_handler.h"

enum
{
    MAX_LINE_LENGTH = 256,
    MAX_ERROR_MESSAGE_LENGTH = 512
};

/**
 * @brief Helper function to process configuration file.
 *
 * @param file Pointer to the opened file.
 * @param config_type Type of configuration (INI or CONF).
 * @return int 0 on success, -1 on error.
 */
static int processConfigFile(FILE *file, const char *config_type)
{
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file))
    {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");
        if (key && value)
        {
            if (validate_string(value) != 0)
            {
                log_error("Invalid %s config value", config_type);
                return -1;
            }
        }
    }

    return 0;
}

/**
 * @brief Load INI configuration file.
 *
 * @param filename Path to the configuration file.
 * @return int 0 on success, -1 on error.
 */
int load_ini_config(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        handle_error("Error opening INI config file");
        return -1;
    }

    int result = processConfigFile(file, "INI");

    if (fclose(file) != 0)
    {
        handle_error("Error closing INI config file");
        return -1;
    }

    return result;
}

/**
 * @brief Load CONF configuration file.
 *
 * @param filename Path to the configuration file.
 * @return int 0 on success, -1 on error.
 */
int load_conf_config(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        handle_error("Error opening CONF config file");
        return -1;
    }

    int result = processConfigFile(file, "CONF");

    if (fclose(file) != 0)
    {
        handle_error("Error closing CONF config file");
        return -1;
    }

    return result;
}
