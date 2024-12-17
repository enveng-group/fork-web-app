/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/process.h */
#ifndef PROCESS_H
#define PROCESS_H

/* Dependencies */
#include "common.h"
#include <pthread.h>

/* Documentation for dependent modules:
 * - Process management and control
 * - Used by scheduler.h for task execution
 * - Provides thread and process abstractions
 */

/* Constants */
#define MAX_PROCESS_NAME 32

/* Process states */
enum process_state {
    PROCESS_CREATED,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED,
    PROCESS_COMPLETED
};

/* Process priority levels */
enum process_priority {
    PRIORITY_LOW,
    PRIORITY_NORMAL,
    PRIORITY_HIGH,
    PRIORITY_REALTIME
};

/* Process structure */
struct process {
    pid_t pid;
    char name[MAX_PROCESS_NAME];
    enum process_state state;
    enum process_priority priority;
    pthread_t thread;
    void *(*task)(void *);
    void *arg;
};

/* Function prototypes */
int processInit(void);
int processCreate(const char *name, void *(*task)(void *), void *arg, enum process_priority priority);
int processTerminate(pid_t pid);
int processWait(pid_t pid);
struct process *processGetCurrent(void);
void processCleanup(void);
int processSetPriority(pid_t pid, enum process_priority priority);
int processGetPriority(pid_t pid);
const char *processGetName(pid_t pid);
enum process_state processGetState(pid_t pid);

#endif /* PROCESS_H */
