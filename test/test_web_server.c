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
#include <sys/time.h>

#define BUFFER_SIZE 1024
#define TEST_PORT 8081
#define TEST_WWW_ROOT "./test"

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
test_handle_file_valid(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    /* Create a socketpair for testing */
    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    /* Prepare test request */
    len = snprintf(request, sizeof(request),
                  "GET /test_index.html HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    /* Send request */
    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    /* Handle request */
    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, 0);

    close(test_client[0]);
    close(test_client[1]);
}

static void
test_handle_file_not_found(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    /* Prepare test request */
    len = snprintf(request, sizeof(request),
                  "GET /nonexistent.html HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    /* Send request */
    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    /* Handle request */
    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, -1);

    close(test_client[0]);
    close(test_client[1]);
}

/* Authentication tests */
static void
test_auth_success(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /auth?username=testuser&password=testpass HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, 0);

    close(test_client[0]);
    close(test_client[1]);
}

/* Record file tests */
static void
test_rec_file_access(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /w6946.rec HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, 0);

    close(test_client[0]);
    close(test_client[1]);
}

/* User management tests */
static void
test_users_listing(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /users HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, 0);

    close(test_client[0]);
    close(test_client[1]);
}

/* Error handling tests */
static void
test_invalid_method(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "POST /index.html HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, -1);

    close(test_client[0]);
    close(test_client[1]);
}

/* Audit log tests */
static void
test_audit_log_access(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /audit_log HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, 0);

    close(test_client[0]);
    close(test_client[1]);
}

/* Edge case tests */
static void
test_buffer_overflow_prevention(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    char long_path[BUFFER_SIZE * 2];
    int len;
    size_t i;

    /* Create extremely long path */
    for (i = 0; i < sizeof(long_path) - 1; i++) {
        long_path[i] = 'A';
    }
    long_path[sizeof(long_path) - 1] = '\0';

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /%s HTTP/1.0\r\n\r\n", long_path);
    CU_ASSERT(len > 0);

    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, -1);  /* Should reject oversized request */

    close(test_client[0]);
    close(test_client[1]);
}

static void
test_path_traversal_prevention(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /../../../etc/passwd HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, -1);  /* Should reject path traversal */

    close(test_client[0]);
    close(test_client[1]);
}

/* Performance tests */
static void
test_concurrent_requests(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;
    int i;
    const int NUM_REQUESTS = 100;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /test_index.html HTTP/1.0\r\n\r\n");
    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

    /* Send multiple rapid requests */
    for (i = 0; i < NUM_REQUESTS; i++) {
        CU_ASSERT(write(test_client[0], request, (size_t)len) == len);
        result = handle_client(test_client[1], TEST_WWW_ROOT);
        CU_ASSERT_EQUAL(result, 0);
    }

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
        (CU_add_test(suite, "Test Handle File Valid", test_handle_file_valid) == NULL) ||
        (CU_add_test(suite, "Test Handle File Not Found", test_handle_file_not_found) == NULL) ||
        (CU_add_test(suite, "Test Authentication Success", test_auth_success) == NULL) ||
        (CU_add_test(suite, "Test Record File Access", test_rec_file_access) == NULL) ||
        (CU_add_test(suite, "Test Users Listing", test_users_listing) == NULL) ||
        (CU_add_test(suite, "Test Invalid Method", test_invalid_method) == NULL) ||
        (CU_add_test(suite, "Test Audit Log Access", test_audit_log_access) == NULL) ||
        /* New edge case and performance tests */
        (CU_add_test(suite, "Test Buffer Overflow Prevention",
                    test_buffer_overflow_prevention) == NULL) ||
        (CU_add_test(suite, "Test Path Traversal Prevention",
                    test_path_traversal_prevention) == NULL) ||
        (CU_add_test(suite, "Test Concurrent Requests",
                    test_concurrent_requests) == NULL) ||
        (CU_add_test(suite, "Test Large File Handling",
                    test_large_file_handling) == NULL)) {
        result = -1;
    } else {
        result = 0;
    }

    return result;
}
