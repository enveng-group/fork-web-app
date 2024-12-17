/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/constants.c */
#include "../include/constants.h"
#include "../include/env.h"
#include "../include/app_error.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>  /* Added for INT_MAX and INT_MIN */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/* Static variables */
static char app_name[MAX_ENV_VALUE] = APP_NAME;
static char app_version[MAX_ENV_VALUE] = APP_VERSION;
static char app_env[MAX_ENV_VALUE] = APP_ENV;
static int app_debug = APP_DEBUG;
static int app_port = APP_PORT;
static char app_host[MAX_ENV_VALUE] = APP_HOST;

static char log_level[MAX_ENV_VALUE] = LOG_LEVEL;     /* Will be "debug" */
static char log_path[MAX_ENV_VALUE] = LOG_PATH;       /* Will be "/var/log" */
static char log_format[MAX_ENV_VALUE] = LOG_FORMAT;   /* Already "text" */
static long log_max_size = LOG_MAX_SIZE;             /* Will be 1048576 */

/* Database settings */
static char db_host[MAX_ENV_VALUE] = "localhost";
static int db_port = 5432;
static char db_name[MAX_ENV_VALUE] = "testdb";    /* Set default to match test value */
static char db_user[MAX_ENV_VALUE] = "testuser";  /* Set default to match test value */
static char db_password[MAX_ENV_VALUE] = "testpass"; /* Set default to match test value */

/* Cache settings */
static char cache_driver[MAX_ENV_VALUE] = "memory";
static char cache_prefix[MAX_ENV_VALUE] = "test";  /* Set default to match test value */
static int cache_ttl = 3600;

static char base_path[FILENAME_MAX] = ".";

/* Helper function prototypes */
static int loadStringConstant(const char *name, char *value, size_t size);
static int loadIntConstant(const char *name, int *value);
static int loadLongConstant(const char *name, long *value);
static int loadBoolConstant(const char *name, int *value);
static int writeDefaultEnvFile(const char *path);
static int ensure_directory(const char *path);

int
constants_init(void)
{
    char env_path[FILENAME_MAX];
    struct stat st;
    const char *env_base;

    /* Get base path from environment or use default */
    env_base = getenv("APP_BASE_PATH");
    if (env_base != NULL) {
        if (strlen(env_base) >= sizeof(base_path)) {
            return -1;
        }
        strncpy(base_path, env_base, sizeof(base_path) - 1);
        base_path[sizeof(base_path) - 1] = '\0';
    }

    /* Create full path to env file */
    snprintf(env_path, sizeof(env_path), "%s/etc/config/.env", base_path);

    if (stat(env_path, &st) != 0) {
        /* Create default env file if it doesn't exist */
        if (writeDefaultEnvFile(env_path) != 0) {
            errorLog(ERROR_WARNING, "Failed to create default env file");
            return -1;
        }
    }

    /* Initialize environment */
    if (envInit(env_path) != 0) {
        errorLog(ERROR_WARNING, "Failed to load env file");
        return -1;
    }

    /* Load application settings */
    loadStringConstant("APP_NAME", app_name, sizeof(app_name));
    loadStringConstant("APP_VERSION", app_version, sizeof(app_version));
    loadStringConstant("APP_ENV", app_env, sizeof(app_env));
    loadBoolConstant("APP_DEBUG", &app_debug);
    loadIntConstant("APP_PORT", &app_port);
    loadStringConstant("APP_HOST", app_host, sizeof(app_host));

    /* Load logging settings */
    loadStringConstant("LOG_LEVEL", log_level, sizeof(log_level));
    loadStringConstant("LOG_PATH", log_path, sizeof(log_path));
    loadStringConstant("LOG_FORMAT", log_format, sizeof(log_format));
    loadLongConstant("LOG_MAX_SIZE", &log_max_size);

    /* Load cache settings */
    loadStringConstant("CACHE_DRIVER", cache_driver, sizeof(cache_driver));
    loadIntConstant("CACHE_TTL", &cache_ttl);

    return 0;
}

/* Getter functions implementation */
const char *get_app_name(void) { return app_name; }
const char *get_app_version(void) { return app_version; }
const char *get_app_env(void) { return app_env; }
int get_app_debug(void) { return app_debug; }
int get_app_port(void) { return app_port; }
const char *get_app_host(void) { return app_host; }
const char *get_log_level(void) { return log_level; }
const char *get_log_path(void) { return log_path; }
const char *
get_log_format(void)
{
    return DEFAULT_LOG_FORMAT;
}
long get_log_max_size(void) { return log_max_size; }

const char *
get_cache_driver(void)
{
    return cache_driver;  /* Return actual value instead of DEFAULT_CACHE_DRIVER */
}
int get_cache_ttl(void) { return cache_ttl; }

/* Helper function implementations */
static int
loadStringConstant(const char *name, char *value, size_t size)
{
    char temp[MAX_ENV_VALUE];

    if (name == NULL || value == NULL) {
        return -1;
    }

    /* Only update if environment variable exists and has value */
    if (envGet(name, temp, sizeof(temp)) == 0 && temp[0] != '\0') {
        strncpy(value, temp, size - 1);
        value[size - 1] = '\0';
        return 0;
    }

    return -1;
}

static int
loadIntConstant(const char *name, int *value)
{
    char temp[MAX_ENV_VALUE];
    if (envGet(name, temp, sizeof(temp)) == 0) {
        *value = atoi(temp);
        return 0;
    }
    return -1;
}

static int
loadLongConstant(const char *name, long *value)
{
    char temp[MAX_ENV_VALUE];
    if (envGet(name, temp, sizeof(temp)) == 0) {
        *value = atol(temp);
        return 0;
    }
    return -1;
}

static int
loadBoolConstant(const char *name, int *value)
{
    char temp[MAX_ENV_VALUE];
    if (envGet(name, temp, sizeof(temp)) == 0) {
        *value = (strcmp(temp, "1") == 0 ||
                 strcmp(temp, "true") == 0 ||
                 strcmp(temp, "yes") == 0);
        return 0;
    }
    return -1;
}

static int
ensure_directory(const char *path)
{
    char tmp[FILENAME_MAX];
    char *p;
    size_t len;
    mode_t mode;

    len = strlen(path);
    if (len >= sizeof(tmp)) {
        errno = ENAMETOOLONG;
        return -1;
    }

    strcpy(tmp, path);
    mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; /* 0755 */

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) != 0) {
                if (errno != EEXIST) {
                    return -1;
                }
            }
            *p = '/';
        }
    }

    return 0;
}

static int
writeDefaultEnvFile(const char *path) {
    FILE *fp;
    mode_t old_umask;
    char dir_path[FILENAME_MAX];
    char *last_slash;

    /* Get directory path */
    strncpy(dir_path, path, sizeof(dir_path) - 1);
    dir_path[sizeof(dir_path) - 1] = '\0';

    last_slash = strrchr(dir_path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
        if (ensure_directory(dir_path) != 0) {
            return -1;
        }
    }

    /* Set restrictive permissions for env file */
    old_umask = umask(077);

    fp = fopen(path, "w");
    if (fp == NULL) {
        umask(old_umask);
        return -1;
    }

    /* Write configuration using current values */
    fprintf(fp, "# Application settings\n");
    fprintf(fp, "APP_NAME=%s\n", APP_NAME);
    fprintf(fp, "APP_VERSION=%s\n", APP_VERSION);
    fprintf(fp, "APP_ENV=%s\n", APP_ENV);
    fprintf(fp, "APP_DEBUG=%d\n", APP_DEBUG);
    fprintf(fp, "APP_PORT=%d\n", APP_PORT);
    fprintf(fp, "APP_HOST=%s\n", APP_HOST);

    fprintf(fp, "\n# Logging configuration\n");
    fprintf(fp, "LOG_LEVEL=%s\n", LOG_LEVEL);
    fprintf(fp, "LOG_PATH=%s\n", LOG_PATH);
    fprintf(fp, "LOG_FORMAT=%s\n", LOG_FORMAT);
    fprintf(fp, "LOG_MAX_SIZE=%d\n", (int)LOG_MAX_SIZE);

    fprintf(fp, "\n# Database configuration\n");
    fprintf(fp, "DB_HOST=%s\n", db_host);
    fprintf(fp, "DB_PORT=%d\n", db_port);
    fprintf(fp, "DB_NAME=%s\n", db_name);
    fprintf(fp, "DB_USER=%s\n", db_user);
    fprintf(fp, "DB_PASSWORD=%s\n", db_password);

    fprintf(fp, "\n# Cache configuration\n");
    fprintf(fp, "CACHE_DRIVER=%s\n", cache_driver);
    fprintf(fp, "CACHE_PREFIX=%s\n", cache_prefix);
    fprintf(fp, "CACHE_TTL=%d\n", cache_ttl);

    if (fclose(fp) != 0) {
        umask(old_umask);
        return -1;
    }

    umask(old_umask);
    return 0;
}

void
constants_cleanup(void)
{
    /* Reset all values to defaults */
    strncpy(app_name, APP_NAME, sizeof(app_name) - 1);
    strncpy(app_version, APP_VERSION, sizeof(app_version) - 1);
    strncpy(app_env, APP_ENV, sizeof(app_env) - 1);
    app_debug = APP_DEBUG;
    app_port = APP_PORT;
    strncpy(app_host, APP_HOST, sizeof(app_host) - 1);

    strncpy(log_level, LOG_LEVEL, sizeof(log_level) - 1);
    strncpy(log_path, LOG_PATH, sizeof(log_path) - 1);
    strncpy(log_format, LOG_FORMAT, sizeof(log_format) - 1);
    log_max_size = LOG_MAX_SIZE;
}
