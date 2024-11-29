/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <stddef.h> // For size_t
#include "config_loader.h" // Include the header where config_t is defined

typedef struct
{
    char *key;
    char *value;
} config_entry_t; // Renamed to avoid conflict with config_t

int validateConfig(config_t *config, const char *value);
int validate_integer(const char *value);
int validate_double(const char *value);
int validate_string(const char *value);
int validate_boolean(const char *value);

// Custom safe copy function
size_t safeCopy(char *dst, const char *src, size_t size);

#endif // VALIDATOR_H
