/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "../include/config_loader.h"
#include "../include/logger.h"
#include "../include/utils.h"
#include "../include/error_codes.h"
#include "../include/constants.h"
#include "../include/env_loader.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
loadIniConfig (FILE *file, Config *config)
{
    char line[MAX_LINE_LENGTH];
    char *key;
    char *value;
    char *saveptr = NULL;
    ConfigEntry entry;

    if (!file)
        {
            logError ("Failed to open config.ini file");
            return -1;
        }

    while (fgets (line, sizeof (line), file))
        {
            char *trimmed_line = trimWhitespace (line);
            if (isEmptyOrComment (trimmed_line))
                {
                    continue;
                }

            logInfo ("Read line: %s", trimmed_line);

            key = strtok_r (trimmed_line, "=", &saveptr);
            value = strtok_r (NULL, "\n", &saveptr);

            if (key && value)
                {
                    key = trimWhitespace (key);
                    value = trimWhitespace (value);

                    logInfo ("Parsed key: %s, value: %s", key, value);

                    entry.key = key;
                    entry.value = value;

                    if (setConfigValue (config, entry) != 0)
                        {
                            logError ("Failed to set config value for key: %s",
                                      key);
                        }
                }
            else
                {
                    logError ("Failed to parse line: %s", trimmed_line);
                }
        }

    logFinalConfig (config);

    return 0;
}

int
isEmptyOrComment (const char *str)
{
    return (str[0] == '\0' || str[0] == '#' || str[0] == ';');
}

int
setConfigValue (Config *config, ConfigEntry entry)
{
    if (strcmp(entry.key, "app_name") == 0)
    {
        safeStrncpy(config->app_name, entry.value, sizeof(config->app_name));
        logInfo("Loaded app_name: %s", config->app_name);
    }
    else if (strcmp(entry.key, "version") == 0)
    {
        char *endptr;
        config->version = strtod(entry.value, &endptr);
        if (*endptr != '\0')
        {
            logError("Invalid version format: %s", entry.value);
        }
        else
        {
            logInfo("Loaded version: %.2f", config->version);
        }
    }
    else if (strcmp(entry.key, "document_root") == 0)
    {
        safeStrncpy(config->document_root, entry.value, sizeof(config->document_root));
        logInfo("Loaded document_root: %s", config->document_root);
    }
    else if (strcmp(entry.key, "rec_file_path") == 0)
    {
        safeStrncpy(config->rec_file_path, entry.value, sizeof(config->rec_file_path));
        logInfo("Loaded rec_file_path: %s", config->rec_file_path);
    }
    else if (strcmp(entry.key, "auth_file") == 0)
    {
        safeStrncpy(config->auth_file, entry.value, sizeof(config->auth_file));
        logInfo("Loaded auth_file: %s", config->auth_file);
    }
    else
    {
        logWarning("Unknown config key: %s", entry.key);
    }

    return SUCCESS;
}

void
logFinalConfig (const Config *config)
{
    logInfo (
        "Final configuration: app_name=%s, version=%.2f, document_root=%s, "
        "rec_file_path=%s, auth_file=%s",
        config->app_name, config->version, config->document_root,
        config->rec_file_path, config->auth_file);
}

void
safeStrncpy (char *dest, const char *src, size_t n)
{
    if (n == 0)
        {
            return;
        }

    while (--n && (*dest++ = *src++))
        {
            ;
        }

    *dest = '\0';
}

int loadConfig(void)
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *key;
    char *value;
    ConfigEntry entry;

    logInfo("Attempting to open config file: %s", CONFIG_FILE);
    file = fopen(CONFIG_FILE, "r");
    if (!file)
    {
        logError("Failed to open config file: %s", CONFIG_FILE);
        return ERROR_FILE_OPEN;
    }

    logInfo("Config file opened successfully: %s", CONFIG_FILE);
    while (fgets(line, sizeof(line), file))
    {
        char *trimmed_line = trimWhitespace(line);
        if (isEmptyOrComment(trimmed_line))
        {
            continue;
        }

        if (trimmed_line[0] == '[' && trimmed_line[strlen(trimmed_line) - 1] == ']')
        {
            logInfo("Skipping section header: %s", trimmed_line);
            continue;
        }

        key = strtok(trimmed_line, "=");
        value = strtok(NULL, "\n");
        if (key && value)
        {
            key = trimWhitespace(key);
            value = trimWhitespace(value);
            entry.key = key;
            entry.value = value;
            logInfo("Setting config value: key=%s, value=%s", key, value);
            if (setConfigValue(&config, entry) != SUCCESS)
            {
                logError("Invalid config value: key=%s, value=%s", key, value);
                fclose(file);
                return ERROR_INVALID_VALUE;
            }
        }
        else
        {
            logError("Failed to parse config line: %s", line);
        }
    }

    fclose(file);
    logInfo("Config file loaded successfully");
    return SUCCESS;
}
