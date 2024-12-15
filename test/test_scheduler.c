/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: test/test_scheduler.c */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <unistd.h>
#include "../include/scheduler.h"
#include "../include/app_error.h"
#include "test_suite.h"

/* Test fixture */
static int test_task_count = 0;
static int test_task_executed = 0;

/* Test task function */
static void *test_task(void *arg)
{
    int *count = (int *)arg;
    (*count)++;
    test_task_executed = 1;
    return NULL;
}

/* Setup and teardown */
static int setup(void)
{
    test_task_count = 0;
    test_task_executed = 0;
    return schedulerInit();
}

static int teardown(void)
{
    schedulerStop();
    schedulerCleanup();
    return 0;
}

/* Test cases */
void test_scheduler_init(void)
{
    int status;

    /* Test double initialization */
    status = schedulerInit();
    CU_ASSERT_EQUAL(status, SCHEDULER_ALREADY_INIT);

    /* Test scheduler state */
    CU_ASSERT_EQUAL(schedulerGetState(), SCHEDULER_STATE_INIT);
}

void test_scheduler_start_stop(void)
{
    int status;

    /* Test start */
    status = schedulerStart();
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);
    CU_ASSERT_EQUAL(schedulerGetState(), SCHEDULER_STATE_RUNNING);

    /* Test double start */
    status = schedulerStart();
    CU_ASSERT_EQUAL(status, SCHEDULER_ALREADY_RUNNING);

    /* Test stop */
    status = schedulerStop();
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);
    CU_ASSERT_EQUAL(schedulerGetState(), SCHEDULER_STATE_STOPPED);

    /* Test double stop */
    status = schedulerStop();
    CU_ASSERT_EQUAL(status, SCHEDULER_ALREADY_STOPPED);
}

void test_scheduler_add_task(void)
{
    struct process proc;
    int status;

    /* Initialize process */
    strncpy(proc.name, "test_task", MAX_PROCESS_NAME - 1);
    proc.state = PROCESS_READY;
    proc.priority = PRIORITY_NORMAL;
    proc.task = test_task;
    proc.arg = &test_task_count;

    /* Test adding task */
    status = schedulerAddTask(&proc);
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);

    /* Test adding NULL task */
    status = schedulerAddTask(NULL);
    CU_ASSERT_EQUAL(status, SCHEDULER_INVALID_TASK);
}

void test_scheduler_remove_task(void)
{
    struct process proc;
    int status;

    /* Initialize process */
    strncpy(proc.name, "test_task", MAX_PROCESS_NAME - 1);
    proc.state = PROCESS_READY;
    proc.priority = PRIORITY_NORMAL;
    proc.task = test_task;
    proc.arg = &test_task_count;

    /* Add a task */
    status = schedulerAddTask(&proc);
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);

    /* Test removing task */
    status = schedulerRemoveTask(proc.pid);
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);

    /* Test removing non-existent task */
    status = schedulerRemoveTask(999);
    CU_ASSERT_EQUAL(status, SCHEDULER_TASK_NOT_FOUND);
}

void test_scheduler_execution(void)
{
    struct process proc;
    int status;

    /* Initialize process */
    strncpy(proc.name, "test_task", MAX_PROCESS_NAME - 1);
    proc.state = PROCESS_READY;
    proc.priority = PRIORITY_HIGH;
    proc.task = test_task;
    proc.arg = &test_task_count;

    /* Add and start task */
    status = schedulerAddTask(&proc);
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);

    status = schedulerStart();
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);

    /* Wait for task execution */
    sleep(1);

    CU_ASSERT_EQUAL(test_task_executed, 1);
    CU_ASSERT(test_task_count > 0);
}

/* Test suite initialization */
int test_scheduler(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Scheduler Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if ((CU_add_test(suite, "Scheduler Initialization", test_scheduler_init) == NULL) ||
        (CU_add_test(suite, "Scheduler Start/Stop", test_scheduler_start_stop) == NULL) ||
        (CU_add_test(suite, "Scheduler Add Task", test_scheduler_add_task) == NULL) ||
        (CU_add_test(suite, "Scheduler Remove Task", test_scheduler_remove_task) == NULL) ||
        (CU_add_test(suite, "Scheduler Execution", test_scheduler_execution) == NULL)) {
        return -1;
    }

    return 0;
}
