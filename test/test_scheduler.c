/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: test/test_scheduler.c */
#include <CUnit/CUnit.h>
#include "../include/scheduler.h"
#include "test_suite.h"
#include <string.h>

/* Test fixtures */
static int test_count = 0;

/* Simple test task */
static void *test_task(void *arg)
{
    if (arg != NULL) {
        (*(int *)arg)++;
    }
    return NULL;
}

/* Test process */
static struct process test_proc;

/* Setup and teardown */
static int setup(void)
{
    test_count = 0;
    return scheduler_init();
}

static int teardown(void)
{
    return scheduler_shutdown();
}

/* Test cases */
void test_scheduler_init(void)
{
    int status = scheduler_init();
    CU_ASSERT_EQUAL(status, SCHEDULER_ERROR_ALREADY_INITIALIZED);
    CU_ASSERT_EQUAL(scheduler_get_state(), SCHEDULER_STATE_STOPPED);
}

void test_scheduler_add_task(void)
{
    int status;

    /* Setup test process */
    memset(&test_proc, 0, sizeof(test_proc));
    test_proc.task = test_task;
    test_proc.arg = &test_count;

    status = scheduler_add_task(&test_proc);
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);

    status = scheduler_start();
    CU_ASSERT_EQUAL(status, SCHEDULER_SUCCESS);
    CU_ASSERT_EQUAL(test_count, 1);
}

/* Test suite initialization */
int test_scheduler(void)
{
    CU_pSuite suite = NULL;

    suite = CU_add_suite("Scheduler Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if ((CU_add_test(suite, "Initialize Scheduler", test_scheduler_init) == NULL) ||
        (CU_add_test(suite, "Add Task", test_scheduler_add_task) == NULL)) {
        return -1;
    }

    return 0;
}
