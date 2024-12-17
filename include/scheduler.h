/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: include/scheduler.h */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

/* Constants */
#define SCHEDULER_SUCCESS 0
#define SCHEDULER_ERROR -1
#define MAX_TASKS 128

/* Error codes */
#define SCHEDULER_ERROR_ALREADY_INITIALIZED -2
#define SCHEDULER_ERROR_NOT_INITIALIZED -3
#define SCHEDULER_ERROR_QUEUE_FULL -4

/* Basic scheduler states */
enum scheduler_state {
    SCHEDULER_STATE_INIT,
    SCHEDULER_STATE_RUNNING,
    SCHEDULER_STATE_STOPPED
};

/* Function prototypes */
int scheduler_init(void);
int scheduler_add_task(struct process *task);
int scheduler_shutdown(void);
int scheduler_start(void);
int scheduler_stop(void);
enum scheduler_state scheduler_get_state(void);

#endif /* SCHEDULER_H */
