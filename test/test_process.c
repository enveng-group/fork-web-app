/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/process.h"
#include "test_suite.h"

/* Test task function */
static void *test_task(void *arg)
{
    (void)arg; /* Unused */
    return NULL;
}

/* Setup and teardown */
static int setup(void)
{
    return processInit();
}

static int teardown(void)
{
    processCleanup();
    return 0;
}

/* Test cases */
void test_process_create(void)
{
    int status = processCreate("test_process", test_task, NULL, PRIORITY_NORMAL);
    CU_ASSERT_EQUAL(status, 0);
}

void test_process_terminate(void)
{
    struct process *proc;
    processCreate("test_process", test_task, NULL, PRIORITY_NORMAL);
    proc = processGetCurrent();
    if (proc != NULL) {
        CU_ASSERT_EQUAL(processTerminate(proc->pid), 0);
    }
}

void test_process_invalid_create(void)
{
    CU_ASSERT_EQUAL(processCreate(NULL, test_task, NULL, PRIORITY_NORMAL), -1);
    CU_ASSERT_EQUAL(processCreate("test", NULL, NULL, PRIORITY_NORMAL), -1);
}

/* Test suite initialization */
int test_process(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Process Management Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if (
        (CU_add_test(suite, "Process Create", test_process_create) == NULL) ||
        (CU_add_test(suite, "Process Terminate", test_process_terminate) == NULL) ||
        (CU_add_test(suite, "Process Invalid Create", test_process_invalid_create) == NULL)
    ) {
        return -1;
    }

    return 0;
}
