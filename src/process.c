/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/process.c */
#include "../include/process.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Static variables */
static struct process process_table[MAX_PROCESSES];
static size_t process_count = 0;
static pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;
static pid_t next_pid = 1;
static pthread_mutex_t pid_mutex = PTHREAD_MUTEX_INITIALIZER;

int
processInit(void)
{
    /* Initialize process table */
    memset(process_table, 0, sizeof(process_table));
    process_count = 0;
    return 0;
}

int
processCreate(const char *name, void *(*task)(void *), void *arg, enum process_priority priority)
{
    struct process *proc;
    int status;
    pid_t new_pid;

    if (name == NULL || task == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Get next available PID atomically */
    pthread_mutex_lock(&pid_mutex);
    new_pid = next_pid++;
    pthread_mutex_unlock(&pid_mutex);

    pthread_mutex_lock(&process_mutex);

    if (process_count >= MAX_PROCESSES) {
        pthread_mutex_unlock(&process_mutex);
        errno = EAGAIN;
        return -1;
    }

    /* Find free slot */
    proc = &process_table[process_count];
    strncpy(proc->name, name, MAX_PROCESS_NAME - 1);
    proc->name[MAX_PROCESS_NAME - 1] = '\0';
    proc->state = PROCESS_READY;
    proc->priority = priority;
    proc->task = task;
    proc->arg = arg;

    /* Create thread for the process */
    status = pthread_create(&proc->thread, NULL, task, arg);
    if (status != 0) {
        pthread_mutex_unlock(&process_mutex);
        errno = status;
        return -1;
    }

    /* Assign the new PID */
    proc->pid = new_pid;
    process_count++;

    pthread_mutex_unlock(&process_mutex);
    return 0;
}

int
processTerminate(pid_t pid)
{
    size_t i;
    int found = 0;

    pthread_mutex_lock(&process_mutex);

    for (i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            pthread_cancel(process_table[i].thread);
            process_table[i].state = PROCESS_TERMINATED;
            found = 1;
            break;
        }
    }

    pthread_mutex_unlock(&process_mutex);

    if (!found) {
        errno = ESRCH;
        return -1;
    }

    return 0;
}

int
processWait(pid_t pid)
{
    size_t i;
    int found = 0;
    void *result;

    pthread_mutex_lock(&process_mutex);

    for (i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            found = 1;
            pthread_mutex_unlock(&process_mutex);
            pthread_join(process_table[i].thread, &result);
            return 0;
        }
    }

    pthread_mutex_unlock(&process_mutex);

    if (!found) {
        errno = ESRCH;
        return -1;
    }

    return 0;
}

struct process *
processGetCurrent(void)
{
    size_t i;
    pthread_t current = pthread_self();

    pthread_mutex_lock(&process_mutex);

    for (i = 0; i < process_count; i++) {
        if (pthread_equal(process_table[i].thread, current)) {
            pthread_mutex_unlock(&process_mutex);
            return &process_table[i];
        }
    }

    pthread_mutex_unlock(&process_mutex);
    return NULL;
}

void
processCleanup(void)
{
    size_t i;

    pthread_mutex_lock(&process_mutex);

    for (i = 0; i < process_count; i++) {
        if (process_table[i].state != PROCESS_TERMINATED) {
            pthread_cancel(process_table[i].thread);
            pthread_join(process_table[i].thread, NULL);
        }
    }

    memset(process_table, 0, sizeof(process_table));
    process_count = 0;

    pthread_mutex_unlock(&process_mutex);
}
