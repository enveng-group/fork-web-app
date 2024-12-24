/* filepath: /home/appuser/web-app/test/test_web_server_security.c */

#include <CUnit/Basic.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "../include/web_server.h"
#include "test_suites.h"

#define BUFFER_SIZE 1024
#define TEST_PORT 8082
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

/* Security Tests */
static void
test_path_traversal(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;
    const char *traversal_paths[] = {
        "/../../../etc/passwd",
        "/..%2F..%2F..%2Fetc%2Fpasswd",
        "/.%2e/.%2e/etc/passwd",
        "/test/../../etc/passwd",
        NULL
    };
    int i;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    for (i = 0; traversal_paths[i] != NULL; i++) {
        len = snprintf(request, sizeof(request),
                      "GET %s HTTP/1.0\r\n\r\n", traversal_paths[i]);
        CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

        CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

        result = handle_client(test_client[1], TEST_WWW_ROOT);
        CU_ASSERT_EQUAL(result, -1);  /* Should reject path traversal */
    }

    close(test_client[0]);
    close(test_client[1]);
}

static void
test_auth_bypass(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;
    const char *bypass_attempts[] = {
        "admin\n:any",     /* Line injection attempt */
        "admin%00",        /* Null byte injection attempt */
        "admin\r",         /* Carriage return injection */
        "%2e%2e%2f",      /* Encoded path traversal attempt */
        NULL
    };
    int i;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    for (i = 0; bypass_attempts[i] != NULL; i++) {
        /* Format complete HTTP request with auth bypass attempt */
        len = snprintf(request, sizeof(request),
                      "GET /login HTTP/1.0\r\n"
                      "Authorization: Basic %s\r\n"
                      "\r\n",
                      bypass_attempts[i]);

        CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

        /* Send request */
        CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

        /* Verify request is rejected */
        result = handle_client(test_client[1], TEST_WWW_ROOT);
        CU_ASSERT_EQUAL(result, -1);
    }

    close(test_client[0]);
    close(test_client[1]);
}

/* Fix test_header_injection */
static void
test_header_injection(void)
{
    int result;
    int test_client[2];
    ssize_t bytes_written;
    const char *injection_headers[] = {
        "GET /test.html HTTP/1.0\r\nInjected: malicious\r\n\r\n",
        "GET /test.html HTTP/1.0\r\n\r\nExtra-Header: bad\r\n\r\n",
        NULL
    };
    int i;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    for (i = 0; injection_headers[i] != NULL; i++) {
        bytes_written = write(test_client[0], injection_headers[i],
                            strlen(injection_headers[i]));
        CU_ASSERT(bytes_written == (ssize_t)strlen(injection_headers[i]));

        result = handle_client(test_client[1], TEST_WWW_ROOT);
        CU_ASSERT_EQUAL(result, -1);  /* Should reject header injection */
    }

    close(test_client[0]);
    close(test_client[1]);
}

/* Fix test_request_smuggling */
static void
test_request_smuggling(void)
{
    int result;
    int test_client[2];
    ssize_t bytes_written;
    const char *smuggling_attempts[] = {
        "GET /test.html HTTP/1.0\r\nContent-Length: 0\r\n\r\nGET /sensitive.html HTTP/1.0\r\n\r\n",
        "GET /test.html HTTP/1.0\r\nTransfer-Encoding: chunked\r\nContent-Length: 5\r\n\r\n0\r\n\r\n",
        NULL
    };
    int i;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    for (i = 0; smuggling_attempts[i] != NULL; i++) {
        bytes_written = write(test_client[0], smuggling_attempts[i],
                            strlen(smuggling_attempts[i]));
        CU_ASSERT(bytes_written == (ssize_t)strlen(smuggling_attempts[i]));

        result = handle_client(test_client[1], TEST_WWW_ROOT);
        CU_ASSERT_EQUAL(result, -1);  /* Should reject request smuggling */
    }

    close(test_client[0]);
    close(test_client[1]);
}

static void
test_sensitive_files(void)
{
    int result;
    int test_client[2];
    char request[BUFFER_SIZE];
    int len;
    const char *sensitive_files[] = {
        "/etc/auth.passwd",
        "/.htaccess",
        "/.git/config",
        "/web_server.c",
        NULL
    };
    int i;

    CU_ASSERT_EQUAL(socketpair(AF_UNIX, SOCK_STREAM, 0, test_client), 0);

    for (i = 0; sensitive_files[i] != NULL; i++) {
        len = snprintf(request, sizeof(request),
                      "GET %s HTTP/1.0\r\n\r\n", sensitive_files[i]);
        CU_ASSERT(len > 0 && (size_t)len < sizeof(request));

        CU_ASSERT(write(test_client[0], request, (size_t)len) == len);

        result = handle_client(test_client[1], TEST_WWW_ROOT);
        CU_ASSERT_EQUAL(result, -1);  /* Should reject access to sensitive files */
    }

    close(test_client[0]);
    close(test_client[1]);
}

/* Add to test suite initialization */
int
init_web_server_security_suite(CU_pSuite suite)
{
    suite->pInitializeFunc = suite_setup;
    suite->pCleanupFunc = suite_teardown;

    if ((CU_add_test(suite, "Test Path Traversal Prevention", test_path_traversal) == NULL) ||
        (CU_add_test(suite, "Test Authentication Bypass Prevention", test_auth_bypass) == NULL) ||
        (CU_add_test(suite, "Test Header Injection Prevention", test_header_injection) == NULL) ||
        (CU_add_test(suite, "Test Sensitive Files Protection", test_sensitive_files) == NULL) ||
        (CU_add_test(suite, "Test Request Smuggling Prevention", test_request_smuggling) == NULL)) {
        return -1;
    }

    return 0;
}
