/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/shell.h */
#ifndef SHELL_H
#define SHELL_H

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64

/* Command status codes */
enum cmd_status {
    CMD_SUCCESS,
    CMD_NOT_FOUND,
    CMD_ERROR
};

/* Function prototypes */
int shellInit(void);
int shellExecuteCommand(const char *command);
void shellPrompt(void);
void shellShutdown(void);

#endif /* SHELL_H */