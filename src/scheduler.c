/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: src/scheduler.c */
#include <string.h>
#include "../include/scheduler.h"

/* Static variables */
static struct process *task_queue[MAX_TASKS];
static size_t task_count = 0;
static int is_initialized = 0;
static enum scheduler_state current_state = SCHEDULER_STATE_INIT;

/* Initialize the scheduler */
int
scheduler_init(void)
{
    if (is_initialized) {
        return SCHEDULER_ERROR_ALREADY_INITIALIZED;
    }

    memset(task_queue, 0, sizeof(task_queue));
    task_count = 0;
    is_initialized = 1;
    current_state = SCHEDULER_STATE_STOPPED;

    return SCHEDULER_SUCCESS;
}

/* Add a task to the queue */
int
scheduler_add_task(struct process *task)
{
    if (!is_initialized) {
        return SCHEDULER_ERROR_NOT_INITIALIZED;
    }

    if (task == NULL) {
        return SCHEDULER_ERROR;
    }

    if (task_count >= MAX_TASKS) {
        return SCHEDULER_ERROR_QUEUE_FULL;
    }

    task_queue[task_count++] = task;
    return SCHEDULER_SUCCESS;
}

/* Process all tasks in queue */
static int
process_tasks(void)
{
    size_t i;
    struct process *task;

    for (i = 0; i < task_count; i++) {
        task = task_queue[i];
        if (task != NULL && task->task != NULL) {
            task->task(task->arg);
            task_queue[i] = NULL;
        }
    }

    task_count = 0;
    return SCHEDULER_SUCCESS;
}

/* Start the scheduler */
int
scheduler_start(void)
{
    if (!is_initialized) {
        return SCHEDULER_ERROR_NOT_INITIALIZED;
    }

    current_state = SCHEDULER_STATE_RUNNING;
    process_tasks();
    return SCHEDULER_SUCCESS;
}

/* Stop the scheduler */
int
scheduler_stop(void)
{
    if (!is_initialized) {
        return SCHEDULER_ERROR_NOT_INITIALIZED;
    }

    current_state = SCHEDULER_STATE_STOPPED;
    return SCHEDULER_SUCCESS;
}

/* Shutdown the scheduler */
int
scheduler_shutdown(void)
{
    if (!is_initialized) {
        return SCHEDULER_ERROR_NOT_INITIALIZED;
    }

    memset(task_queue, 0, sizeof(task_queue));
    task_count = 0;
    is_initialized = 0;
    current_state = SCHEDULER_STATE_INIT;

    return SCHEDULER_SUCCESS;
}

/* Get current scheduler state */
enum scheduler_state
scheduler_get_state(void)
{
    return current_state;
}
