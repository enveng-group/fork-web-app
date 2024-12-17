/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
/* filepath: test/test_consistency.c */
#include <CUnit/CUnit.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include "../include/constants.h"
#include "../include/common.h"
#include "../include/version.h"
#include "../include/env.h"
#include "test_suite.h"

/* Function prototypes */
void test_path_consistency(void);
void test_buffer_sizes(void);
void test_version_consistency(void);
void test_env_consistency(void);
void test_path_validation(void);      /* Changed return type to void */
void test_path_normalization(void);   /* Changed return type to void */
static int create_test_files(void);
int init_consistency_suite(void);
int init_path_validation_suite(void);

/* Helper function to create test files */
static int
create_test_files(void)
{
    FILE *fp;
    mode_t old_umask;
    struct stat st = {0};

    /* Create test directory if it doesn't exist */
    if (stat("test", &st) == -1) {
        if (mkdir("test", 0755) == -1) {
            return -1;
        }
    }

    /* Set less restrictive permissions for test files */
    old_umask = umask(0022);

    /* Create test.conf if it doesn't exist */
    if (stat(DEFAULT_CONFIG_PATH, &st) == -1) {
        fp = fopen(DEFAULT_CONFIG_PATH, "w");
        if (fp == NULL) {
            umask(old_umask);
            return -1;
        }
        fprintf(fp, "# Test configuration file\n");
        fclose(fp);
    }

    /* Create test.log if it doesn't exist */
    if (stat(DEFAULT_LOG_PATH, &st) == -1) {
        fp = fopen(DEFAULT_LOG_PATH, "w");
        if (fp == NULL) {
            umask(old_umask);
            return -1;
        }
        fprintf(fp, "# Test log file\n");
        fclose(fp);
    }

    /* Create test.env if it doesn't exist */
    if (stat(DEFAULT_ENV_PATH, &st) == -1) {
        fp = fopen(DEFAULT_ENV_PATH, "w");
        if (fp == NULL) {
            umask(old_umask);
            return -1;
        }
        fprintf(fp, "APP_NAME=%s\n", APP_NAME);
        fprintf(fp, "APP_VERSION=%s\n", APP_VERSION);
        fprintf(fp, "APP_ENV=%s\n", APP_ENV);
        fprintf(fp, "APP_DEBUG=%d\n", APP_DEBUG);
        fprintf(fp, "APP_PORT=%d\n", APP_PORT);
        fprintf(fp, "APP_HOST=%s\n", APP_HOST);
        fclose(fp);
    }

    umask(old_umask);
    return 0;
}

/* Test implementations */
void
test_path_consistency(void)
{
    struct stat st = {0};

    /* Create test files if they don't exist */
    CU_ASSERT_EQUAL(create_test_files(), 0);

    /* Test existence of configuration paths */
    CU_ASSERT_EQUAL(stat(DEFAULT_CONFIG_PATH, &st), 0);
    CU_ASSERT_EQUAL(stat(DEFAULT_LOG_PATH, &st), 0);
    CU_ASSERT_EQUAL(stat(DEFAULT_ENV_PATH, &st), 0);
}

void
test_buffer_sizes(void)
{
    /* Test buffer size relationships */
    CU_ASSERT(MAX_LINE_LENGTH >= 0);
    CU_ASSERT(MAX_ENV_VALUE >= MAX_ENV_KEY);
    CU_ASSERT(MAX_CONFIG_LINE >= MAX_LINE_LENGTH);
}

void
test_version_consistency(void)
{
    /* Test API version consistency */
    CU_ASSERT_EQUAL(CACHE_API_VERSION, CACHE_API_VERSION_REQUIRED);
    CU_ASSERT_EQUAL(MEM_API_VERSION, MEM_API_VERSION_REQUIRED);
}

void
test_env_consistency(void)
{
    char value[MAX_ENV_VALUE];
    int result;

    /* Create test files if they don't exist */
    CU_ASSERT_EQUAL(create_test_files(), 0);

    /* Initialize environment with test file */
    result = envInit(DEFAULT_ENV_PATH);
    CU_ASSERT_EQUAL(result, 0);
    if (result != 0) {
        return;
    }

    /* Test APP_NAME */
    memset(value, 0, sizeof(value));
    result = envGet("APP_NAME", value, sizeof(value));
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_STRING_EQUAL(value, APP_NAME);

    /* Test APP_VERSION */
    memset(value, 0, sizeof(value));
    result = envGet("APP_VERSION", value, sizeof(value));
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_STRING_EQUAL(value, APP_VERSION);

    /* Clean up */
    envCleanup();
}

void
test_path_validation(void)            /* Implementation as void */
{
    /* Add test implementation */
    CU_ASSERT(1);  /* Placeholder assertion */
}

void
test_path_normalization(void)         /* Implementation as void */
{
    /* Add test implementation */
    CU_ASSERT(1);  /* Placeholder assertion */
}

/* Test suite initialization */
int
init_consistency_suite(void)
{
    CU_pSuite suite = NULL;

    suite = CU_add_suite("Consistency Tests", NULL, NULL);
    if (NULL == suite) {
        return -1;
    }

    if ((NULL == CU_add_test(suite, "Path Consistency", test_path_consistency)) ||
        (NULL == CU_add_test(suite, "Buffer Sizes", test_buffer_sizes)) ||
        (NULL == CU_add_test(suite, "Version Consistency", test_version_consistency)) ||
        (NULL == CU_add_test(suite, "Environment Consistency", test_env_consistency))) {
        return -1;
    }

    return 0;
}

int
init_path_validation_suite(void)
{
    CU_pSuite suite = NULL;

    suite = CU_add_suite("Path Validation Tests", NULL, NULL);
    if (suite == NULL) {
        return -1;
    }

    if (CU_add_test(suite, "Test Path Validation", test_path_validation) == NULL ||
        CU_add_test(suite, "Test Path Normalization", test_path_normalization) == NULL) {
        return -1;
    }

    return 0;
}
