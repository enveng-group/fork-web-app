/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/scheduler.c */
#include "../include/scheduler.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Static variables */
static struct process *task_queue[MAX_TASKS];
static size_t task_count = 0;
static enum scheduler_state current_state = SCHEDULER_STATE_STOPPED;
static pthread_mutex_t scheduler_mutex = PTHREAD_MUTEX_INITIALIZER;
static int is_initialized = 0;

int
schedulerInit(void)
{
    pthread_mutex_lock(&scheduler_mutex);

    /* Check if already initialized */
    if (is_initialized) {
        pthread_mutex_unlock(&scheduler_mutex);
        return SCHEDULER_ALREADY_INIT;
    }

    memset(task_queue, 0, sizeof(task_queue));
    task_count = 0;
    current_state = SCHEDULER_STATE_INIT;
    is_initialized = 1;

    pthread_mutex_unlock(&scheduler_mutex);
    return SCHEDULER_SUCCESS;
}

int
schedulerStart(void)
{
    size_t i;

    pthread_mutex_lock(&scheduler_mutex);

    if (current_state == SCHEDULER_STATE_RUNNING) {
        pthread_mutex_unlock(&scheduler_mutex);
        return SCHEDULER_ALREADY_RUNNING;
    }

    current_state = SCHEDULER_STATE_RUNNING;

    /* Execute tasks */
    i = 0;
    while (i < task_count) {
        if (task_queue[i] && task_queue[i]->task) {
            task_queue[i]->state = PROCESS_RUNNING;
            task_queue[i]->task(task_queue[i]->arg);
        }
        i++;
    }

    pthread_mutex_unlock(&scheduler_mutex);
    return SCHEDULER_SUCCESS;
}

int
schedulerStop(void)
{
    pthread_mutex_lock(&scheduler_mutex);

    if (current_state == SCHEDULER_STATE_STOPPED) {
        pthread_mutex_unlock(&scheduler_mutex);
        return SCHEDULER_ALREADY_STOPPED;
    }

    current_state = SCHEDULER_STATE_STOPPED;

    pthread_mutex_unlock(&scheduler_mutex);
    return SCHEDULER_SUCCESS;
}

int
schedulerPause(void)
{
    pthread_mutex_lock(&scheduler_mutex);
    if (current_state != SCHEDULER_STATE_RUNNING) {
        pthread_mutex_unlock(&scheduler_mutex);
        errno = EINVAL;
        return SCHEDULER_ERROR;
    }
    current_state = SCHEDULER_STATE_PAUSED;
    pthread_mutex_unlock(&scheduler_mutex);
    return SCHEDULER_SUCCESS;
}

int
schedulerResume(void)
{
    pthread_mutex_lock(&scheduler_mutex);
    if (current_state != SCHEDULER_STATE_PAUSED) {
        pthread_mutex_unlock(&scheduler_mutex);
        errno = EINVAL;
        return SCHEDULER_ERROR;
    }
    current_state = SCHEDULER_STATE_RUNNING;
    pthread_mutex_unlock(&scheduler_mutex);
    return SCHEDULER_SUCCESS;
}

int
schedulerAddTask(struct process *proc)
{
    if (proc == NULL) {
        return SCHEDULER_INVALID_TASK;
    }

    pthread_mutex_lock(&scheduler_mutex);

    if (task_count >= MAX_TASKS) {
        pthread_mutex_unlock(&scheduler_mutex);
        errno = EAGAIN;
        return SCHEDULER_ERROR;
    }

    task_queue[task_count++] = proc;
    proc->state = PROCESS_READY;

    pthread_mutex_unlock(&scheduler_mutex);
    return SCHEDULER_SUCCESS;
}

int
schedulerRemoveTask(pid_t pid)
{
    size_t i;
    int found;

    found = 0;
    pthread_mutex_lock(&scheduler_mutex);

    i = 0;
    while (i < task_count) {
        if (task_queue[i]->pid == pid) {
            /* Shift remaining tasks */
            if (i < task_count - 1) {
                memmove(&task_queue[i], &task_queue[i + 1],
                        (task_count - i - 1) * sizeof(struct process *));
            }
            task_count--;
            found = 1;
            break;
        }
        i++;
    }

    pthread_mutex_unlock(&scheduler_mutex);

    if (!found) {
        return SCHEDULER_TASK_NOT_FOUND;
    }

    return SCHEDULER_SUCCESS;
}

enum scheduler_state
schedulerGetState(void)
{
    enum scheduler_state state;
    pthread_mutex_lock(&scheduler_mutex);
    state = current_state;
    pthread_mutex_unlock(&scheduler_mutex);
    return state;
}

void
schedulerCleanup(void)
{
    pthread_mutex_lock(&scheduler_mutex);
    memset(task_queue, 0, sizeof(task_queue));
    task_count = 0;
    current_state = SCHEDULER_STATE_STOPPED;
    is_initialized = 0;
    pthread_mutex_unlock(&scheduler_mutex);
}
