/**
 * \file validator.h
 * \brief Header file for configuration validation functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "config_loader.h"

/**
 * \brief Validates the configuration.
 *
 * \param config Configuration structure to validate.
 * \param value Value to validate.
 * \return 0 if validation is successful, otherwise an error code.
 */
int validateConfig (Config *config, const char *value);

#endif /* VALIDATOR_H */
