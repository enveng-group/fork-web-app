/* filepath: /home/appuser/web-app/test/test_web_server.c */
/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include <CUnit/Basic.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "../include/web_server.h"
#include "test_suites.h"

#define BUFFER_SIZE 1024
#define TEST_PORT 8081

/* Test fixture */
static int test_socket;

/* Setup and teardown */
static int
suite_setup(void)
{
    test_socket = setup_server(TEST_PORT);
    return (test_socket < 0) ? -1 : 0;
}

static int
suite_teardown(void)
{
    if (test_socket >= 0) {
        close(test_socket);
    }
    return 0;
}

/* Test cases */
static void
test_server_setup(void)
{
    int socket_fd;

    socket_fd = setup_server(TEST_PORT + 1);
    CU_ASSERT(socket_fd >= 0);

    if (socket_fd >= 0) {
        close(socket_fd);
    }
}

static void
test_serve_file_valid(void)
{
    ssize_t result;
    int test_client[2];
    const char *test_uri = "/test_index.html";

    /* Create a socketpair for testing */
    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    result = serve_file(test_client[1], test_uri);
    CU_ASSERT(result >= 0);

    close(test_client[0]);
    close(test_client[1]);
}

static void
test_serve_file_not_found(void)
{
    ssize_t result;
    int test_client[2];
    const char *test_uri = "/nonexistent.html";

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    result = serve_file(test_client[1], test_uri);
    CU_ASSERT(result < 0);

    close(test_client[0]);
    close(test_client[1]);
}

/* Test suite initialization */
int
init_web_server_suite(CU_pSuite suite)
{
    int result;

    /* Set up the suite setup and teardown functions */
    suite->pInitializeFunc = suite_setup;
    suite->pCleanupFunc = suite_teardown;

    /* Add tests */
    if ((CU_add_test(suite, "Test Server Setup", test_server_setup) == NULL) ||
        (CU_add_test(suite, "Test Serve File Valid", test_serve_file_valid) == NULL) ||
        (CU_add_test(suite, "Test Serve File Not Found", test_serve_file_not_found) == NULL)) {
        result = -1;
    } else {
        result = 0;
    }

    return result;
}
