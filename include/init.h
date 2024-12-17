/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef INIT_H
#define INIT_H

/* System states */
enum system_state {
    STATE_STARTUP,
    STATE_RUNNING,
    STATE_SHUTDOWN,
    STATE_ERROR,
    STATE_REBOOT
};

/* Return codes */
#define INIT_SUCCESS 0
#define INIT_ERROR  -1
#define INIT_ERR_ALREADY_RUNNING -2

/* Function prototypes */
int initSystem(void);
int shutdownSystem(void);
int rebootSystem(void);
int loadConfiguration(const char *config_path);
enum system_state getSystemState(void);

#endif /* INIT_H */
