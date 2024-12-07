/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef ENV_LOADER_H
#define ENV_LOADER_H

#include <stdio.h>

typedef struct
{
    char *key;
    char *value;
} env_var_t;

int loadEnvConfig(const char *filename);

#endif  // ENV_LOADER_H
