/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "config_loader.h"

size_t utils_safeCopy(char *dst, const char *src, size_t size);
int    utils_processConfigFile(FILE *file, const char *config_type);
int    utils_loadEnvConfig(const char *filename);

#endif  // UTILS_H
