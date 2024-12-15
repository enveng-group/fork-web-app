/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/init.h */
#ifndef INIT_H
#define INIT_H

#include <sys/types.h>
#include <unistd.h>

/* Constants */
#define INIT_SUCCESS 0
#define INIT_ERROR -1
#define MAX_ENV_VARS 100
#define MAX_PATH_LEN 4096
#define MAX_LINE_LEN 1024

/* System states */
enum system_state {
    STATE_STARTUP,
    STATE_RUNNING,
    STATE_SHUTDOWN,
    STATE_REBOOT,
    STATE_ERROR
};

/* Function prototypes */
int initSystem(void);
int shutdownSystem(void);
int rebootSystem(void);
int loadConfiguration(const char *config_path);
enum system_state getSystemState(void);

#endif /* INIT_H */
