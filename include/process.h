/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/process.h */
#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include <pthread.h>

#define MAX_PROCESSES 64
#define MAX_PROCESS_NAME 32

/* Process states */
enum process_state {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
};

/* Process priority levels */
enum process_priority {
    PRIORITY_LOW,
    PRIORITY_NORMAL,
    PRIORITY_HIGH
};

/* Process structure */
struct process {
    pid_t pid;                          /* Process ID */
    char name[MAX_PROCESS_NAME];        /* Process name */
    enum process_state state;           /* Current state */
    enum process_priority priority;      /* Process priority */
    pthread_t thread;                   /* Associated thread */
    void *(*task)(void *);             /* Task function */
    void *arg;                         /* Task argument */
};

/* Function prototypes */
int processInit(void);
int processCreate(const char *name, void *(*task)(void *), void *arg, enum process_priority priority);
int processTerminate(pid_t pid);
int processWait(pid_t pid);
struct process *processGetCurrent(void);
void processCleanup(void);

#endif /* PROCESS_H */
