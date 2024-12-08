/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

/* Function declarations */
int validateInput(const char *input);
char *trimWhitespace(char *str);

size_t utils_safeCopy (char *dst, const char *src, size_t size);
int utils_processConfigFile (FILE *file, const char *config_type);
int utils_loadEnvConfig (const char *filename);

#endif /* UTILS_H */
