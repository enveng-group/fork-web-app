/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef ENV_LOADER_H
#define ENV_LOADER_H

typedef struct
{
    char *key;
    char *value;
} env_var_t;

/* Function declarations */
void loadEnvironmentVariables(void);
const char *getEnvVariable(const char *name);
char *trimWhitespace(char *str);
int loadEnvConfig(const char *filename);  /* Add this declaration */

#endif /* ENV_LOADER_H */
