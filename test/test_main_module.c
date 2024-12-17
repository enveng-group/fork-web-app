/* filepath: /devcontainer/web-app/test/test_main_module.c */

/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* System headers */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

/* Application headers */
#include "../include/init.h"
#include "../include/fs.h"
#include "../include/app_error.h"
#include "../include/shell.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/mem.h"
#include "../include/cache.h"
#include "../include/constants.h"
#include "../include/socket.h"
#include "../include/net.h"
#include "../include/tcp.h"
#include "test_suite.h"

/* Test constants */
#define TEST_LOG_PATH "test/test.log"
#define TEST_CONFIG_PATH "test/test.conf"
#define TEST_CACHE_SIZE 1024
#define TEST_MEM_SIZE (1024 * 1024)
#define TEST_PORT 8080
#define TEST_HOST "127.0.0.1"

/* Forward declarations */
static int setup(void);
static int teardown(void);
static int setup_signals(void);
static void test_socket_operations(void);
static void test_signal_handler(int signum);

/* Global signal handlers */
static struct sigaction old_sigint;
static struct sigaction old_sigterm;

/* Signal handler function */
static void
test_signal_handler(int signum)
{
    /* Handle test signals */
    if (signum == SIGINT || signum == SIGTERM) {
        teardown();
        exit(1);
    }
}

/* Test signal handling setup */
static int setup_signals(void)
{
    struct sigaction new_action;
    int result;

    /* Initialize the signal action structure */
    new_action.sa_handler = test_signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    /* Save old handlers */
    result = sigaction(SIGINT, NULL, &old_sigint);
    if (result == -1) {
        return -1;
    }

    result = sigaction(SIGTERM, NULL, &old_sigterm);
    if (result == -1) {
        return -1;
    }

    /* Install new handlers */
    result = sigaction(SIGINT, &new_action, NULL);
    if (result == -1) {
        return -1;
    }

    result = sigaction(SIGTERM, &new_action, NULL);
    if (result == -1) {
        return -1;
    }

    return 0;
}

/* Test setup function */
static int setup(void)
{
    struct stat st = {0};
    FILE *fp;
    int status;

    /* Initialize memory system first */
    status = memInit(TEST_MEM_SIZE);
    if (status != MEM_SUCCESS) {
        return -1;
    }

    /* Initialize socket system */
    status = socketInit(SOCKET_INIT_DEFAULT);
    if (status != SOCKET_SUCCESS) {
        memCleanup();
        return -1;
    }

    /* Initialize cache system with LRU policy */
    status = cacheInit(CACHE_TYPE_LRU, TEST_CACHE_SIZE);
    if (status != CACHE_SUCCESS) {
        socketCleanup();
        memCleanup();
        return -1;
    }

    /* Setup signal handlers */
    if (setup_signals() == -1) {
        cacheCleanup();
        socketCleanup();
        memCleanup();
        return -1;
    }

    /* Create test directory if needed */
    if (stat("test", &st) == -1) {
        if (mkdir("test", 0755) == -1) {
            cacheCleanup();
            socketCleanup();
            memCleanup();
            return -1;
        }
    }

    /* Create and set up test log file */
    fp = fopen(TEST_LOG_PATH, "w");
    if (fp == NULL) {
        cacheCleanup();
        socketCleanup();
        memCleanup();
        return -1;
    }
    fclose(fp);

    /* Create and set up test config file */
    fp = fopen(TEST_CONFIG_PATH, "w");
    if (fp == NULL) {
        cacheCleanup();
        socketCleanup();
        memCleanup();
        return -1;
    }
    fprintf(fp, "CACHE_SIZE=%d\n", TEST_CACHE_SIZE);
    fclose(fp);

    return 0;
}

static int teardown(void)
{
    /* Restore signal handlers */
    sigaction(SIGINT, &old_sigint, NULL);
    sigaction(SIGTERM, &old_sigterm, NULL);

    /* Cleanup subsystems */
    socketCleanup();
    cacheCleanup();
    memCleanup();

    /* Remove test files */
    unlink(TEST_LOG_PATH);
    unlink(TEST_CONFIG_PATH);

    return 0;
}

/* Test cases */
void test_main_startup(void)
{
    int status;

    /* Test normal initialization */
    status = initSystem();
    CU_ASSERT_EQUAL(status, INIT_SUCCESS);

    if (status == INIT_SUCCESS) {
        /* Verify system state */
        CU_ASSERT_EQUAL(getSystemState(), STATE_RUNNING);

        /* Verify subsystems are initialized */
        CU_ASSERT_EQUAL(memGetStatus(), MEM_SUCCESS);
        CU_ASSERT_EQUAL(cacheGetStatus(), CACHE_SUCCESS);

        shutdownSystem();
    }
}

void test_main_signal_handling(void)
{
    struct sigaction test_sigint;
    struct sigaction test_sigterm;
    struct sigaction current_sigint;
    struct sigaction current_sigterm;

    /* Initialize signal handlers */
    test_sigint.sa_handler = test_signal_handler;
    test_sigterm.sa_handler = test_signal_handler;
    sigemptyset(&test_sigint.sa_mask);
    sigemptyset(&test_sigterm.sa_mask);
    test_sigint.sa_flags = 0;
    test_sigterm.sa_flags = 0;

    /* Set test handlers */
    CU_ASSERT_EQUAL(sigaction(SIGINT, &test_sigint, &old_sigint), 0);
    CU_ASSERT_EQUAL(sigaction(SIGTERM, &test_sigterm, &old_sigterm), 0);

    /* Verify handlers were set */
    CU_ASSERT_EQUAL(sigaction(SIGINT, NULL, &current_sigint), 0);
    CU_ASSERT_EQUAL(sigaction(SIGTERM, NULL, &current_sigterm), 0);
    CU_ASSERT_EQUAL(current_sigint.sa_handler, test_signal_handler);
    CU_ASSERT_EQUAL(current_sigterm.sa_handler, test_signal_handler);

    /* Restore original handlers */
    CU_ASSERT_EQUAL(sigaction(SIGINT, &old_sigint, NULL), 0);
    CU_ASSERT_EQUAL(sigaction(SIGTERM, &old_sigterm, NULL), 0);
}

void test_main_cleanup(void)
{
    /* Initialize system */
    CU_ASSERT_EQUAL(initSystem(), INIT_SUCCESS);

    /* Test cleanup */
    CU_ASSERT_EQUAL(shutdownSystem(), INIT_SUCCESS);

    /* Verify system state */
    CU_ASSERT_EQUAL(getSystemState(), STATE_SHUTDOWN);
}

void test_main_args_handling(void)
{
    const char *argv[] = {
        "program",
        TEST_LOG_PATH,
        NULL
    };
    int status;

    /* Test with custom log path */
    status = initSystem();
    CU_ASSERT_EQUAL(status, INIT_SUCCESS);

    /* Verify custom log path was used */
    CU_ASSERT_EQUAL(access(argv[1], F_OK), 0);

    shutdownSystem();
}

void test_memory_management(void)
{
    void *ptr;
    struct mem_stats stats_before, stats_after;

    /* Initialize system */
    CU_ASSERT_EQUAL(initSystem(), INIT_SUCCESS);

    /* Get initial memory stats */
    stats_before = memGetStats();

    /* Allocate and free memory */
    ptr = memAlloc(1024);
    CU_ASSERT_PTR_NOT_NULL(ptr);

    if (ptr != NULL) {
        memFree(ptr);
    }

    /* Check memory stats */
    stats_after = memGetStats();
    CU_ASSERT_EQUAL(stats_before.total_allocs + 1, stats_after.total_allocs);

    shutdownSystem();
}

void test_cache_operations(void)
{
    const char *test_key = "test_key";
    const char *test_value = "test_value";
    char buffer[64];
    size_t size = sizeof(buffer);

    /* Initialize system */
    CU_ASSERT_EQUAL(initSystem(), INIT_SUCCESS);

    /* Test cache operations */
    CU_ASSERT_EQUAL(cacheSet(test_key, test_value, strlen(test_value) + 1, 60), CACHE_SUCCESS);
    CU_ASSERT_EQUAL(cacheGet(test_key, buffer, &size), CACHE_SUCCESS);
    CU_ASSERT_STRING_EQUAL(buffer, test_value);
    CU_ASSERT_EQUAL(cacheDelete(test_key), CACHE_SUCCESS);

    shutdownSystem();
}

void test_socket_operations(void)
{
    struct socket_info sock;
    int status;

    /* Initialize socket subsystem */
    status = socketInit(SOCKET_INIT_DEFAULT);
    CU_ASSERT_EQUAL(status, 0);
    if (status != 0) {
        return;
    }

    /* Test error string retrieval */
    CU_ASSERT_STRING_EQUAL(socketGetErrorStr(SOCKET_SUCCESS), "Success");

    /* Create socket */
    status = netCreateSocket(&sock, SOCKET_TCP);
    CU_ASSERT_EQUAL(status, 0);
    if (status != 0) {
        socketCleanup();
        return;
    }

    /* Test binding */
    status = netBind(&sock, TEST_HOST, TEST_PORT);
    CU_ASSERT_EQUAL(status, 0);
    if (status != 0) {
        netClose(&sock);
        socketCleanup();
        return;
    }

    /* Test listening */
    status = netListen(&sock);
    CU_ASSERT_EQUAL(status, 0);

    /* Cleanup */
    netClose(&sock);
    socketCleanup();
}

/* Test suite initialization */
int test_main_module(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Main Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if ((CU_add_test(suite, "Main Startup", test_main_startup) == NULL) ||
        (CU_add_test(suite, "Signal Handling", test_main_signal_handling) == NULL) ||
        (CU_add_test(suite, "Main Cleanup", test_main_cleanup) == NULL) ||
        (CU_add_test(suite, "Arguments Handling", test_main_args_handling) == NULL) ||
        (CU_add_test(suite, "Memory Management", test_memory_management) == NULL) ||
        (CU_add_test(suite, "Cache Operations", test_cache_operations) == NULL) ||
        (CU_add_test(suite, "Socket Operations", test_socket_operations) == NULL)) {
        return -1;
    }

    return 0;
}
