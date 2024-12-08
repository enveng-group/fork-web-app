/**
 * \file config_loader.c
 * \brief Implements configuration loading functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/config_loader.h"
#include "../include/constants.h"
#include "../include/env_loader.h"
#include "../include/error_codes.h"
#include "../include/garbage_collector.h"
#include "../include/logger.h"
#include "../include/utils.h"
#include "../include/data_structures.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *configPath = NULL;
int logLevel = 0;
int maxConnections = 0;
int timeout = 0;

/**
 * \brief Checks for integer overflow in addition.
 *
 * \param a First integer.
 * \param b Second integer.
 * \return 1 if overflow occurs, otherwise 0.
 */
int
checkAdditionOverflow (int a, int b)
{
    if ((b > 0) && (a > (INT_MAX - b)))
        {
            return 1; /* Overflow */
        }
    if ((b < 0) && (a < (INT_MIN - b)))
        {
            return 1; /* Underflow */
        }
    return 0; /* No overflow */
}

/**
 * \brief Sets a configuration value.
 *
 * \param config Configuration structure.
 * \param entry Configuration entry.
 * \return 0 on success, otherwise an error code.
 */
int setConfigValue(Config *config, ConfigEntry entry)
{
    if (config == NULL || entry.key == NULL || entry.value == NULL)
    {
        logError("Invalid config, key, or value");
        return -1;
    }
    /* Implementation of setting the configuration value */
    return 0;
}

/**
 * \brief Loads configuration from an INI file.
 *
 * \param filename Name of the INI file.
 * \return 0 on success, otherwise an error code.
 */
int loadIniConfig(const char *filename)
{
    if (filename == NULL)
    {
        logError("Invalid filename");
        return -1;
    }

    logInfo("Loading configuration from %s", filename);

    /* Load configuration logic here */

    return 0;
}

/**
 * \brief Logs the final configuration.
 *
 * \param config Configuration structure.
 */
void logFinalConfig(const Config *config)
{
    if (config == NULL)
    {
        logError("Invalid config");
        return;
    }
    /* Implementation of logging the final configuration */
}

/**
 * \brief Loads configuration from an INI file.
 *
 * \param filename Name of the INI file.
 * \param config Pointer to the Config structure to load the configuration into.
 * \return 0 on success, otherwise an error code.
 */
int loadConfig(const char *filename, Config *config)
{
    (void)filename; /* Suppress unused parameter warning */

    if (config == NULL)
    {
        logError("Invalid config pointer");
        return -1;
    }

    /* Implementation of loading the configuration */

    return 0;
}
