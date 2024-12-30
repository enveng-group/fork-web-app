/* filepath: /home/appuser/web-app/test/test_suites.h */
/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

#ifndef TEST_SUITES_H
#define TEST_SUITES_H

/* System includes with guards */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

#include <CUnit/Basic.h>

/* Constants */
#define BUFFER_SIZE 1024
#define MAX_PATH 4096
#define MAX_LOG_SIZE 1024
#define MAX_BUFFER_SIZE 4096
#define TEST_AUTH_FILE "test/test_auth.passwd"
#define TEST_PORT 8081
#define TEST_PORT_SECURITY 8082
#define CONCURRENT_CLIENTS 5

/* Path constants */
#define TEST_WWW_ROOT "./test"
#define TEST_REC "test/test.rec"
#define AUDIT_LOG "test/test.log"

/* Error codes */
#define ERR_NONE 0
#define TEST_ERR_PARAM -1
#define TEST_ERR_AUTH -2
#define TEST_ERR_IO -3

/* Function prototypes */
int init_web_server_suite(CU_pSuite suite);
int init_web_server_security_suite(CU_pSuite suite);

#endif /* TEST_SUITES_H */
