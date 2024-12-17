/* filepath: test/test_init.c */
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/init.h"
#include "../include/app_error.h"
#include "../include/env.h"        /* Add this for env functions */
#include "../include/constants.h"
#include "../include/config.h"
#include "../include/fs.h"
#include "test_suite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/* Static function prototypes */
static int cleanup_test_files(void);
static int setup(void);
static int teardown(void);

/* Test directories */
static const char *test_dirs[] = {
    "test_tmp",                  /* Base test directory */
    "test_tmp/etc",             /* Config directory */
    "test_tmp/etc/config",      /* App configuration */
    "test_tmp/var",             /* Variable data */
    "test_tmp/var/log",         /* Log files */
    "test_tmp/var/run",         /* Runtime data */
    NULL
};

/* Required test files - relative paths */
static const char *test_files[] = {
    "test_tmp/etc/config/.env",
    "test_tmp/etc/config/system.conf",
    "test_tmp/var/log/app.log",
    NULL
};

/* Forward declarations of helper functions */
static int createTestDirectories(void) __attribute__((unused));
static int createEnvFile(void) __attribute__((unused));
static int createTestConfig(void) __attribute__((unused));
static int createTestEnv(void) __attribute__((unused));

static int createEnvFile(void)
{
    FILE *fp;
    const char *env_path = "test_tmp/etc/config/.env";
    mode_t old_umask;

    /* Set restrictive permissions during file creation */
    old_umask = umask(077);

    fp = fopen(env_path, "w");
    if (fp == NULL) {
        umask(old_umask);
        return -1;
    }

    /* Write required environment variables */
    fprintf(fp, "APP_NAME=TestApp\n");
    fprintf(fp, "APP_VERSION=1.0.0\n");
    fprintf(fp, "APP_ENV=test\n");
    fprintf(fp, "APP_DEBUG=1\n");
    fprintf(fp, "APP_PORT=8080\n");
    fprintf(fp, "APP_HOST=localhost\n");
    fprintf(fp, "LOG_LEVEL=debug\n");
    fprintf(fp, "LOG_PATH=test_tmp/var/log/app.log\n");
    fprintf(fp, "LOG_FORMAT=text\n");
    fprintf(fp, "LOG_MAX_SIZE=1048576\n");
    fprintf(fp, "CACHE_DRIVER=memory\n");
    fprintf(fp, "CACHE_TTL=3600\n");

    fclose(fp);
    umask(old_umask);

    /* Set file permissions */
    if (chmod(env_path, 0600) != 0) {
        return -1;
    }

    return 0;
}

/* Update directory creation/cleanup functions to use test_dirs */
static int createTestDirectories(void)
{
    const char **dir;
    mode_t old_umask;

    old_umask = umask(022);

    /* Create test directories */
    for (dir = test_dirs; *dir != NULL; dir++) {
        if (mkdir(*dir, 0755) != 0 && errno != EEXIST) {
            umask(old_umask);
            return -1;
        }
    }

    umask(old_umask);
    return 0;
}

static int createTestConfig(void)
{
    FILE *fp;
    const char *config_path = "test_tmp/etc/config/system.conf";
    mode_t old_umask;

    old_umask = umask(077);

    fp = fopen(config_path, "w");
    if (fp == NULL) {
        umask(old_umask);
        return -1;
    }

    /* Write standard test configuration */
    fprintf(fp, "APP_NAME=TestApp\n");
    fprintf(fp, "APP_VERSION=1.0.0\n");
    fprintf(fp, "APP_ENV=test\n");
    fprintf(fp, "LOG_PATH=test_tmp/var/log/app.log\n");

    fclose(fp);
    umask(old_umask);

    return 0;
}

static int createTestEnv(void)
{
    FILE *fp;
    mode_t old_umask;

    /* Create test environment file */
    old_umask = umask(022);

    fp = fopen("test_tmp/etc/config/.env", "w");
    if (fp == NULL) {
        umask(old_umask);
        return -1;
    }

    /* Write test environment variables */
    fprintf(fp, "APP_NAME=TestApp\n");
    fprintf(fp, "APP_VERSION=1.0.0\n");
    fprintf(fp, "APP_ENV=test\n");
    fprintf(fp, "APP_DEBUG=1\n");
    fprintf(fp, "APP_PORT=8080\n");
    fprintf(fp, "APP_HOST=localhost\n");
    fprintf(fp, "LOG_LEVEL=debug\n");
    fprintf(fp, "LOG_PATH=test_tmp/var/log\n");
    fprintf(fp, "LOG_FORMAT=text\n");
    fprintf(fp, "LOG_MAX_SIZE=1048576\n");

    fclose(fp);
    umask(old_umask);
    return 0;
}

/* Helper function to clean up test files */
static int cleanup_test_files(void)
{
    const char **file;
    int result = 0;

    for (file = test_files; *file != NULL; file++) {
        if (unlink(*file) != 0 && errno != ENOENT) {
            result = -1;
        }
    }
    return result;
}

/* Update directory creation/cleanup functions to use test_dirs */
static int teardown(void)
{
    size_t i;
    int result = 0;

    /* First clean up files */
    result = cleanup_test_files();

    /* Then remove directories in reverse order */
    for (i = 0; test_dirs[i] != NULL; i++)
        ; /* Count directories */

    while (i > 0) {
        i--;
        if (rmdir(test_dirs[i]) != 0 && errno != ENOENT) {
            result = -1;
        }
    }

    return result;
}

/* Test fixture setup/teardown functions */
static int setup(void)
{
    const char **dir;

    for (dir = test_dirs; *dir != NULL; dir++) {
        if (mkdir(*dir, 0755) != 0 && errno != EEXIST) {
            return -1;
        }
    }
    return 0;
}

/* Test cases */
/* Update setenv calls */
void test_system_initialization(void)
{
    int status;

    status = initSystem();
    CU_ASSERT_EQUAL_FATAL(status, INIT_SUCCESS);

    if (status == INIT_SUCCESS) {
        CU_ASSERT_EQUAL(getSystemState(), STATE_RUNNING);
        shutdownSystem();
    }
}

void test_config_loading(void)
{
    int status;

    status = initSystem();
    CU_ASSERT_EQUAL_FATAL(status, INIT_SUCCESS);
    CU_ASSERT_EQUAL(getSystemState(), STATE_RUNNING);

    shutdownSystem();
}

/* Update setenv calls */
void test_system_shutdown(void)
{
    int status;

    status = initSystem();
    CU_ASSERT_EQUAL_FATAL(status, INIT_SUCCESS);

    status = shutdownSystem();
    CU_ASSERT_EQUAL(status, INIT_SUCCESS);
    CU_ASSERT_EQUAL(getSystemState(), STATE_SHUTDOWN);
}

/* Update setenv calls */
void test_init_system(void)
{
    int status;

    status = initSystem();
    CU_ASSERT_EQUAL_FATAL(status, INIT_SUCCESS);
    CU_ASSERT_EQUAL(getSystemState(), STATE_RUNNING);

    shutdownSystem();
}

/* Test suite initialization */
int test_init(void)
{
    CU_pSuite suite = NULL;

    /* Create suite with setup/teardown */
    suite = CU_add_suite("Init Module Tests", setup, teardown);
    if (suite == NULL) {
        fprintf(stderr, "Failed to create init test suite\n");
        return -1;
    }

    /* Add test cases */
    if ((CU_add_test(suite, "System Initialization", test_system_initialization) == NULL) ||
        (CU_add_test(suite, "Configuration Loading", test_config_loading) == NULL) ||
        (CU_add_test(suite, "System Shutdown", test_system_shutdown) == NULL) ||
        (CU_add_test(suite, "Init System", test_init_system) == NULL)) {
        fprintf(stderr, "Failed to add init test cases\n");
        return -1;
    }

    return 0;
}
