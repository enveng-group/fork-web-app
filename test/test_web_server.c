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
#include <signal.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <sys/resource.h>
#include <time.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define TEST_PORT 8081
#define TEST_WWW_ROOT "./test"

#ifndef CONCURRENT_CLIENTS
#define CONCURRENT_CLIENTS 50
#endif

/* Add error tracking */
struct error_metrics {
    int auth_failures;
    int invalid_requests;
    int file_errors;
    int connection_errors;
} errors;

/* Add signal handler function */
static volatile sig_atomic_t signal_received = 0;

static void
test_signal_handler(int signo)
{
    signal_received = signo;
}

/* Replace the existing test_signal_handling function */
static void
test_signal_handling(void)
{
    struct sigaction sa;
    struct sigaction old_sa;
    int result;

    /* Save old signal handler */
    memset(&old_sa, 0, sizeof(old_sa));
    memset(&sa, 0, sizeof(sa));

    /* Set up signal handler */
    sa.sa_handler = test_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /* Install signal handler */
    if (sigaction(SIGTERM, &sa, &old_sa) < 0) {
        CU_FAIL("Failed to install signal handler");
        return;
    }

    /* Reset signal received flag */
    signal_received = 0;

    /* Send signal to self */
    result = raise(SIGTERM);
    CU_ASSERT_EQUAL(result, 0);

    /* Verify signal was caught */
    CU_ASSERT_EQUAL(signal_received, SIGTERM);

    /* Restore original signal handler */
    if (sigaction(SIGTERM, &old_sa, NULL) < 0) {
        CU_FAIL("Failed to restore signal handler");
    }
}

/* Helper function to track error rates during testing */
static void
track_error_rates(int result, int expected) __attribute__((unused));

static void
track_error_rates(int result, int expected)
{
    if (result != expected) {
        switch (result) {
            case ERR_AUTH:
                errors.auth_failures++;
                break;
            case ERR_IO:
                errors.file_errors++;
                break;
            default:
                errors.invalid_requests++;
        }
    }
}

/* Test fixture */
static int test_socket;

/* Modify suite_setup to include signal handler setup */
static int
suite_setup(void)
{
    struct sigaction sa;

    /* Set up signal handlers */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = test_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        return -1;
    }

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

/* Record file tests */
static void
test_rec_file_access(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;
    FILE *test_file;

    /* Create test record file */
    test_file = fopen(W6946_REC, "w");
    if (test_file) {
        fprintf(test_file, "Project_Name: W6946\nTest_Data: test\n");
        fclose(test_file);
    }

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    /* Add proper authorization header */
    len = snprintf(request, sizeof(request),
                  "GET /w6946.rec HTTP/1.0\r\n"
                  "Authorization: Basic dGVzdHVzZXI6dGVzdHBhc3M=\r\n"
                  "Accept: text/plain\r\n"
                  "\r\n");

    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));
    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, 0);

    close(test_client[0]);
    close(test_client[1]);
}

static void
test_auth_success(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    /* Use existing testuser from auth.passwd */
    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /auth HTTP/1.0\r\n"
                  "Authorization: Basic dGVzdHVzZXI6eA==\r\n" /* base64(testuser:x) */
                  "\r\n");

    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));
    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, 0);

    close(test_client[0]);
    close(test_client[1]);
}

static void
test_users_listing(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;

    /* Use existing admin from auth.passwd */
    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    len = snprintf(request, sizeof(request),
                  "GET /users HTTP/1.0\r\n"
                  "Authorization: Basic YWRtaW46eA==\r\n" /* base64(admin:x) */
                  "\r\n");

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

    /* Create test audit log */
    FILE *log_file = fopen(AUDIT_LOG, "w");
    if (log_file) {
        fprintf(log_file, "2024-01-01 00:00:00|test|action|message\n");
        fclose(log_file);
    }

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    /* Add admin authorization */
    len = snprintf(request, sizeof(request),
                  "GET /audit_log HTTP/1.0\r\n"
                  "Authorization: Basic YWRtaW46YWRtaW4xMjM=\r\n"
                  "\r\n");

    CU_ASSERT(len > 0 && (size_t)len < sizeof(request));
    CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, 0);

    close(test_client[0]);
    close(test_client[1]);
}

static void
test_buffer_overflow_prevention(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    char path_component[16]; /* Small fixed buffer for repeatable component */
    size_t total_len = 0;
    size_t remaining;
    int len;

    /* Initialize buffers */
    memset(request, 0, sizeof(request));
    memset(path_component, 'A', sizeof(path_component) - 1);
    path_component[sizeof(path_component) - 1] = '\0';

    /* Input validation */
    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);
    if (test_client[0] < 0 || test_client[1] < 0) {
        CU_FAIL("Failed to create socket pair");
        return;
    }

    /* Construct request with controlled buffer size */
    len = snprintf(request, sizeof(request), "GET /");
    if (len < 0 || (size_t)len >= sizeof(request)) {
        close(test_client[0]);
        close(test_client[1]);
        CU_FAIL("Initial request formatting failed");
        return;
    }

    total_len = (size_t)len;
    remaining = sizeof(request) - total_len - 1; /* -1 for null terminator */

    /* Add path components until we reach MAX_PATH */
    while (total_len < MAX_PATH && remaining > sizeof(path_component)) {
        len = snprintf(request + total_len, remaining, "%s", path_component);
        if (len < 0 || (size_t)len >= remaining) {
            break;
        }
        total_len += (size_t)len;
        remaining = sizeof(request) - total_len - 1;
    }

    /* Add HTTP version */
    len = snprintf(request + total_len, remaining, " HTTP/1.0\r\n\r\n");
    if (len < 0 || (size_t)len >= remaining) {
        close(test_client[0]);
        close(test_client[1]);
        CU_FAIL("Failed to append HTTP version");
        return;
    }

    /* Send request */
    if (write(test_client[0], request, strlen(request)) != (ssize_t)strlen(request)) {
        close(test_client[0]);
        close(test_client[1]);
        CU_FAIL("Failed to write request");
        return;
    }

    /* Handle request - should reject oversized path */
    result = handle_client(test_client[1], TEST_WWW_ROOT);
    CU_ASSERT_EQUAL(result, -1);

    /* Cleanup */
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

static void
test_parse_query_string(void)
{
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];

    /* Test valid query */
    parse_query_string("username=admin&password=secret", username, password);
    CU_ASSERT_STRING_EQUAL(username, "admin");
    CU_ASSERT_STRING_EQUAL(password, "secret");

    /* Test empty query */
    parse_query_string("", username, password);
    CU_ASSERT_STRING_EQUAL(username, "");
    CU_ASSERT_STRING_EQUAL(password, "");
}

static void
test_parse_auth_file(void)
{
    struct user_entry entries[10];
    int count;

    count = parse_auth_file(AUTH_FILE, entries, 10);
    CU_ASSERT(count > 0);
    CU_ASSERT_STRING_NOT_EQUAL(entries[0].username, "");
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

struct log_metrics {
    int rotations;
    size_t total_log_size;
    int max_backup_files;
} log_stats;

static void
track_log_metrics(void)
{
    struct stat st;
    int ret;

    ret = stat(AUDIT_LOG, &st);
    if (ret == 0) {
        /* Check for negative size */
        if (st.st_size < 0) {
            errno = EINVAL;
            return;
        }
        /* Safe conversion after validation */
        log_stats.total_log_size = (size_t)st.st_size;
        CU_ASSERT(log_stats.total_log_size < MAX_LOG_SIZE);
    }
}

static void
test_log_message(void)
{
    int result;
    const char *username = "testuser";
    const char *action = "TEST_ACTION";
    const char *msg = "Test message";

    /* Test valid parameters */
    result = log_message(LOG_INFO, username, action, msg);
    CU_ASSERT_EQUAL(result, ERR_NONE);

    result = log_message(LOG_ERROR, username, action, msg);
    CU_ASSERT_EQUAL(result, ERR_NONE);

    /* Test NULL parameters - should return ERR_PARAM */
    result = log_message(LOG_INFO, NULL, action, msg);
    CU_ASSERT_EQUAL(result, ERR_PARAM);

    result = log_message(LOG_INFO, username, NULL, msg);
    CU_ASSERT_EQUAL(result, ERR_PARAM);

    result = log_message(LOG_INFO, username, action, NULL);
    CU_ASSERT_EQUAL(result, ERR_PARAM);
}

static void
test_record_operations(void)
{
    int result;
    const char *test_data = "test_data";
    int client_socket[2];

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, client_socket), 0);

    /* Test create record */
    result = handle_create_record(client_socket[1], test_data);
    CU_ASSERT_EQUAL(result, ERR_NONE);

    close(client_socket[0]);
    close(client_socket[1]);
}

struct server_metrics {
    double avg_response_time;
    double max_response_time;
    int requests_per_second;
    int concurrent_connections;
} metrics;

/* Helper function for safe time difference calculation */
static double
time_diff(const struct timeval *end, const struct timeval *start)
{
    long sec_diff;
    long usec_diff;
    double elapsed;

    sec_diff = end->tv_sec - start->tv_sec;
    usec_diff = end->tv_usec - start->tv_usec;

    if (usec_diff < 0) {
        --sec_diff;
        usec_diff += 1000000;
    }

    elapsed = (double)sec_diff + ((double)usec_diff / 1000000.0);
    return elapsed;
}

static void
track_performance_metrics(struct timeval *start, struct timeval *end)
{
    double elapsed;

    if (start == NULL || end == NULL) {
        return;
    }

    elapsed = time_diff(end, start);

    /* Avoid division by zero */
    if (elapsed > 0.0) {
        metrics.avg_response_time = elapsed / (double)CONCURRENT_CLIENTS;
        metrics.requests_per_second = (int)(((double)CONCURRENT_CLIENTS) / elapsed);
    } else {
        metrics.avg_response_time = 0.0;
        metrics.requests_per_second = 0;
    }

    CU_ASSERT(metrics.avg_response_time < 0.1);  /* 100ms max per request */
    CU_ASSERT(metrics.requests_per_second > 10);  /* Minimum 10 RPS */
}

/* Helper function for safe time calculations */
static double
safe_time_diff(const struct timeval *end, const struct timeval *start)
{
    double sec_diff;
    double usec_diff;

    sec_diff = (double)(end->tv_sec - start->tv_sec);
    usec_diff = (double)(end->tv_usec - start->tv_usec) / 1000000.0;

    return sec_diff + usec_diff;
}

static void
test_server_load(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;
    int i;
    struct timeval start;
    struct timeval end;
    double elapsed;

    /* Start timing */
    if (gettimeofday(&start, NULL) != 0) {
        CU_FAIL("Failed to get start time");
        return;
    }

    for (i = 0; i < CONCURRENT_CLIENTS; i++) {
        CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

        len = snprintf(request, sizeof(request),
                      "GET /test_index.html HTTP/1.0\r\n\r\n");
        CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

        CU_ASSERT(write(test_client[0], request, (size_t)len) == len);
        result = handle_client(test_client[1], TEST_WWW_ROOT);
        CU_ASSERT_EQUAL(result, 0);

        close(test_client[0]);
        close(test_client[1]);
    }

    /* End timing */
    if (gettimeofday(&end, NULL) != 0) {
        CU_FAIL("Failed to get end time");
        return;
    }

    /* Calculate elapsed time using helper function */
    elapsed = safe_time_diff(&end, &start);

    /* Assert reasonable response time */
    CU_ASSERT(elapsed < 5.0); /* Should handle requests within 5 seconds */

    track_performance_metrics(&start, &end);
}

struct connection_metrics {
    int successful_connections;
    int failed_connections;
    double avg_connection_time;
} connections;

/* Test suite initialization */
int
init_web_server_suite(CU_pSuite suite)
{
    int result;

    /* Initialize metrics */
    memset(&metrics, 0, sizeof(metrics));
    memset(&errors, 0, sizeof(errors));
    memset(&connections, 0, sizeof(connections));
    memset(&log_stats, 0, sizeof(log_stats));

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
        (CU_add_test(suite, "Test Parse Query String",
                    test_parse_query_string) == NULL) ||
        (CU_add_test(suite, "Test Parse Auth File",
                    test_parse_auth_file) == NULL) ||
        (CU_add_test(suite, "Test Concurrent Requests",
                    test_concurrent_requests) == NULL) ||
        (CU_add_test(suite, "Test Signal Handling",
                    test_signal_handling) == NULL) ||
        (CU_add_test(suite, "Test Log Message",
                    test_log_message) == NULL) ||
        (CU_add_test(suite, "Test Record Operations",
                    test_record_operations) == NULL) ||
        (CU_add_test(suite, "Test Log Message", test_log_message) == NULL) ||
        (CU_add_test(suite, "Test Parse Query String", test_parse_query_string) == NULL) ||
        (CU_add_test(suite, "Test Record Operations", test_record_operations) == NULL) ||
        (CU_add_test(suite, "Test Auth File Parsing", test_parse_auth_file) == NULL) ||
        (CU_add_test(suite, "Test Server Load", test_server_load) == NULL) ||
        (CU_add_test(suite, "Test Log Metrics", track_log_metrics) == NULL)) {
        result = -1;
    } else {
        result = 0;
    }

    return result;
}
