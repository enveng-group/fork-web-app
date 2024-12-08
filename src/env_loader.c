/**
 * \file env_loader.c
 * \brief Implements environment variable loading functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/env_loader.h"
#include "../include/logger.h"
#include "../include/utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * \enum LINE_SIZE
 * \brief Defines the maximum size of a line in the environment file.
 */
enum
{
    LINE_SIZE = 1024
};

/**
 * \brief Loads environment variables from the environment file.
 */
void
loadEnvironmentVariables (void)
{
    /* Implementation of loading environment variables */
}

/**
 * \brief Loads environment configuration from a file.
 *
 * \param filename Name of the file to load.
 * \return 0 if loading is successful, otherwise an error code.
 */
int
loadEnvConfig (const char *filename)
{
    FILE *file;
    char line[1024];
    char key[256], value[256];

    file = fopen (filename, "r");
    if (file == NULL)
        {
            logError ("Failed to open environment file");
            return -1;
        }

    while (fgets (line, sizeof (line), file))
        {
            if (sscanf (line, "%255[^=]=%255[^\n]", key, value) == 2)
                {
                    setenv (trimWhitespace (key), trimWhitespace (value), 1);
                    logInfo ("Loaded environment variable: %s=%s", key, value);
                }
        }

    fclose (file);
    return 0;
}
