/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "config_loader.h"
#include "validator.h"
#include "logger.h"
#include "error_handler.h"

#define MAX_LINE_LENGTH           1024
#define MAX_ERROR_MESSAGE_LENGTH  256

/**
 * @brief Helper function to process configuration file.
 *
 * @param file Pointer to the opened file.
 * @param config_type Type of configuration (INI or CONF).
 * @return int 0 on success, -1 on error.
 */
int processConfigFile(FILE *file, const char *config_type)
{
    char line[256];

    while (fgets(line, sizeof(line), file))
    {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");
        if (key && value)
        {
            config_t temp_config;
            safeCopy(temp_config.app_name, key, sizeof(temp_config.app_name));
            temp_config.version = 1; // Assuming a default version for validation

            if (validateConfig(&temp_config, value) != 0)
            {
                char error_message[MAX_ERROR_MESSAGE_LENGTH];
                snprintf(error_message, sizeof(error_message), "Invalid %s config value", config_type);
                logError(error_message);
                return -1;
            }
        }
    }
    return 0;
}

int loadIniConfig(const char *filename, config_t *config)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        logError("Failed to open config.ini file: %s", filename);
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        // Trim leading and trailing whitespace
        char *trimmed_line = line;
        while (isspace((unsigned char)*trimmed_line)) trimmed_line++;
        size_t len = strlen(trimmed_line);
        if (len == 0)
        {
            continue;
        }
        char *end = trimmed_line + len - 1;
        while (end > trimmed_line && isspace((unsigned char)*end)) end--;
        end[1] = '\0';

        // Skip empty lines and comments
        if (trimmed_line[0] == '\0' || trimmed_line[0] == '#' || trimmed_line[0] == ';')
        {
            continue;
        }

        // Skip section headers
        if (trimmed_line[0] == '[' && trimmed_line[strlen(trimmed_line) - 1] == ']')
        {
            continue;
        }

        logInfo("Read line: %s", trimmed_line);

        char *key = strtok(trimmed_line, "=");
        char *value = strtok(NULL, "\n");

        if (key && value)
        {
            // Trim leading and trailing whitespace from key and value
            while (isspace((unsigned char)*key)) key++;
            end = key + strlen(key) - 1;
            while (end > key && isspace((unsigned char)*end)) end--;
            end[1] = '\0';

            while (isspace((unsigned char)*value)) value++;
            end = value + strlen(value) - 1;
            while (end > value && isspace((unsigned char)*end)) end--;
            end[1] = '\0';

            logInfo("Parsed key: %s, value: %s", key, value);

            if (strcmp(key, "app_name") == 0)
            {
                strncpy(config->app_name, value, sizeof(config->app_name) - 1);
                config->app_name[sizeof(config->app_name) - 1] = '\0'; // Ensure null-termination
                logInfo("Loaded app_name: %s", config->app_name);
            }
            else if (strcmp(key, "version") == 0)
            {
                config->version = atof(value);
                logInfo("Loaded version: %.2f", config->version);
            }
            else if (strcmp(key, "document_root") == 0)
            {
                strncpy(config->document_root, value, sizeof(config->document_root) - 1);
                config->document_root[sizeof(config->document_root) - 1] = '\0'; // Ensure null-termination
                logInfo("Loaded document_root: %s", config->document_root);
            }
            else if (strcmp(key, "rec_file_path") == 0)
            {
                strncpy(config->rec_file_path, value, sizeof(config->rec_file_path) - 1);
                config->rec_file_path[sizeof(config->rec_file_path) - 1] = '\0'; // Ensure null-termination
                logInfo("Loaded rec_file_path: %s", config->rec_file_path);
            }
            else if (strcmp(key, "auth_file") == 0)
            {
                strncpy(config->auth_file, value, sizeof(config->auth_file) - 1);
                config->auth_file[sizeof(config->auth_file) - 1] = '\0'; // Ensure null-termination
                logInfo("Loaded auth_file: %s", config->auth_file);
            }
        }
        else
        {
            logError("Failed to parse line: %s", trimmed_line);
        }
    }

    // Log the final configuration values for debugging
    logInfo("Final configuration: app_name=%s, version=%.2f, document_root=%s, rec_file_path=%s, auth_file=%s",
            config->app_name, config->version, config->document_root, config->rec_file_path, config->auth_file);

    fclose(file);
    return 0;
}

void someFunction(void)
{
    // Function implementation
}
