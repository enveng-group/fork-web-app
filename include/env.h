/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/env.h */
#ifndef ENV_H
#define ENV_H

#include <sys/types.h>
#include "constants.h"

/* Constants */
#define MAX_ENV_VARS 100

/* Environment variable status codes */
enum env_status {
    ENV_SUCCESS,
    ENV_NOT_FOUND,
    ENV_INVALID_PARAM,
    ENV_ERROR
};

/* Function prototypes */
int envInit(const char *env_path);
int envGet(const char *name, char *value, size_t size);
int envSet(const char *name, const char *value);
void envCleanup(void);
enum env_status envGetStatus(void);

#endif /* ENV_H */
