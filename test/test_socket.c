/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* test/test_socket.c */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/socket.h"
#include "../include/net.h"
#include "../include/socket.h"

/* Function prototypes */
static void test_socket_init(void);
static void test_socket_error_str(void);
static void test_socket_init_flags(void);
static int test_socket_setup(void);
static int test_socket_teardown(void);
int test_socket(void);

/* Test setup function */
static int
test_socket_setup(void)
{
    return 0;
}

/* Test teardown function */
static int
test_socket_teardown(void)
{
    return 0;
}

/* Test fixture */
static void
test_socket_init(void)
{
    int result;

    /* Test normal initialization */
    result = socketInit(SOCKET_INIT_DEFAULT);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(socketGetLastError(), SOCKET_SUCCESS);

    /* Cleanup */
    socketCleanup();

    /* Test double initialization */
    result = socketInit(SOCKET_INIT_DEFAULT);
    CU_ASSERT_EQUAL(result, 0);
    socketCleanup();
}

/* Test error string function */
static void
test_socket_error_str(void)
{
    const char *error_str;

    error_str = socketGetErrorStr(SOCKET_SUCCESS);
    CU_ASSERT_STRING_EQUAL(error_str, "Success");

    error_str = socketGetErrorStr(SOCKET_ERROR_INIT);
    CU_ASSERT_STRING_EQUAL(error_str, "Socket initialization failed");
}

/* Test socket initialization flags */
static void
test_socket_init_flags(void)
{
    int result;

    /* Test non-blocking initialization */
    result = socketInit(SOCKET_INIT_NONBLOCK);
    CU_ASSERT_EQUAL(result, 0);
    socketCleanup();

    /* Test address reuse initialization */
    result = socketInit(SOCKET_INIT_REUSE_ADDR);
    CU_ASSERT_EQUAL(result, 0);
    socketCleanup();

    /* Test combined flags */
    result = socketInit(SOCKET_INIT_NONBLOCK | SOCKET_INIT_REUSE_ADDR);
    CU_ASSERT_EQUAL(result, 0);
    socketCleanup();
}

/* Register all socket tests */
int
test_socket(void)
{
    /* Initialize the CUnit test registry */
    CU_pSuite pSuite = NULL;

    /* Add a suite to the registry */
    pSuite = CU_add_suite("Socket Test Suite", test_socket_setup, test_socket_teardown);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test socket initialization", test_socket_init)) ||
        (NULL == CU_add_test(pSuite, "test socket error strings", test_socket_error_str)) ||
        (NULL == CU_add_test(pSuite, "test socket initialization flags", test_socket_init_flags)))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return CU_get_error();
}
