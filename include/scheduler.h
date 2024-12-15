/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/scheduler.h */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

/* Constants */
#define SCHEDULER_SUCCESS 0
#define SCHEDULER_ERROR -1
#define MAX_TASKS 128

/* Error codes */
#define SCHEDULER_ALREADY_INIT -2
#define SCHEDULER_ALREADY_RUNNING -3
#define SCHEDULER_ALREADY_STOPPED -4
#define SCHEDULER_INVALID_TASK -5
#define SCHEDULER_INVALID_INTERVAL -6
#define SCHEDULER_TASK_NOT_FOUND -7

/* Scheduler states */
enum scheduler_state {
    SCHEDULER_STATE_INIT,
    SCHEDULER_STATE_STOPPED,
    SCHEDULER_STATE_RUNNING,
    SCHEDULER_STATE_PAUSED
};

/* Function prototypes */
int schedulerInit(void);
int schedulerStart(void);
int schedulerStop(void);
int schedulerPause(void);
int schedulerResume(void);
int schedulerAddTask(struct process *proc);
int schedulerRemoveTask(pid_t pid);
enum scheduler_state schedulerGetState(void);
void schedulerCleanup(void);

#endif /* SCHEDULER_H */
