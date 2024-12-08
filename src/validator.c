/**
 * \file validator.c
 * \brief Implements configuration validation functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/config_loader.h"
#include "../include/logger.h" /* Include logger.h for logError */
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * \brief Validates the configuration.
 *
 * \param config Configuration structure to validate.
 * \param value Value to validate.
 * \return 0 if validation is successful, otherwise an error code.
 */
int validateConfig(const Config *config, const char *value)
{
    if (config == NULL || value == NULL)
    {
        logError("Invalid configuration or value");
        return -1;
    }

    /* Validation logic here */

    return 0;
}
