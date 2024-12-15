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

/* Application constants derived from environment */
static char app_name[MAX_ENV_VALUE];
static char app_version[MAX_ENV_VALUE];
static char app_env[MAX_ENV_VALUE];
static int app_debug;
static int app_port;
static char app_host[MAX_ENV_VALUE];

/* Logging constants */
static char log_level[MAX_ENV_VALUE];
static char log_path[MAX_ENV_VALUE];
static char log_format[MAX_ENV_VALUE];
static long log_max_size;

/* Database constants */
static char db_host[MAX_ENV_VALUE];
static int db_port;
static char db_name[MAX_ENV_VALUE];
static char db_user[MAX_ENV_VALUE];
static char db_password[MAX_ENV_VALUE];

/* Cache constants */
static char cache_driver[MAX_ENV_VALUE];
static char cache_prefix[MAX_ENV_VALUE];
static int cache_ttl;

/* Helper function prototypes */
static int loadStringConstant(const char *name, char *value, size_t size);
static int loadIntConstant(const char *name, int *value);
static int loadLongConstant(const char *name, long *value);
static int loadBoolConstant(const char *name, int *value);

int
constants_init(void)  /* Changed from constantsInit */
{
    int status;

    /* Skip environment initialization if it's already done */
    if (envGetStatus() != ENV_SUCCESS) {
        status = envInit(DEFAULT_ENV_PATH);
        if (status != 0) {
            /* Just log warning and continue with defaults */
            errorLog(ERROR_WARNING, "Failed to initialize environment");
        }
    }

    /* Initialize all values with defaults first */
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

    /* Try to load from environment, but don't fail if not found */
    loadStringConstant("APP_NAME", app_name, sizeof(app_name));
    loadStringConstant("APP_VERSION", app_version, sizeof(app_version));
    loadStringConstant("APP_ENV", app_env, sizeof(app_env));
    loadBoolConstant("APP_DEBUG", &app_debug);
    loadIntConstant("APP_PORT", &app_port);
    loadStringConstant("APP_HOST", app_host, sizeof(app_host));
    loadStringConstant("LOG_LEVEL", log_level, sizeof(log_level));
    loadStringConstant("LOG_PATH", log_path, sizeof(log_path));
    loadStringConstant("LOG_FORMAT", log_format, sizeof(log_format));
    loadLongConstant("LOG_MAX_SIZE", &log_max_size);

    /* Load database constants */
    status |= loadStringConstant("DB_HOST", db_host, sizeof(db_host));
    status |= loadIntConstant("DB_PORT", &db_port);
    status |= loadStringConstant("DB_NAME", db_name, sizeof(db_name));
    status |= loadStringConstant("DB_USER", db_user, sizeof(db_user));
    status |= loadStringConstant("DB_PASSWORD", db_password, sizeof(db_password));

    /* Load cache constants */
    status |= loadStringConstant("CACHE_DRIVER", cache_driver, sizeof(cache_driver));
    status |= loadStringConstant("CACHE_PREFIX", cache_prefix, sizeof(cache_prefix));
    status |= loadIntConstant("CACHE_TTL", &cache_ttl);

    return EXIT_SUCCESS;
}

/* Getter functions for application constants */
const char *
get_app_name(void)  /* Changed from getAppName */
{
    return app_name;
}

const char *
get_app_version(void)  /* Changed from getAppVersion */
{
    return app_version;
}

const char *
get_app_env(void)  /* Changed from getAppEnv */
{
    return app_env;
}

int
get_app_debug(void)  /* Changed from getAppDebug */
{
    return app_debug;
}

int
get_app_port(void)  /* Changed from getAppPort */
{
    return app_port;
}

const char *
get_app_host(void)  /* Changed from getAppHost */
{
    return app_host;
}

/* Getter functions for logging constants */
const char *
get_log_level(void)  /* Changed from getLogLevel */
{
    return log_level;
}

const char *
get_log_path(void)  /* Changed from getLogPath */
{
    return log_path;
}

const char *
get_log_format(void)  /* Changed from getLogFormat */
{
    return log_format;
}

long
get_log_max_size(void)  /* Changed from getLogMaxSize */
{
    return log_max_size;
}

/* Helper functions for loading different types of constants */
static int
loadStringConstant(const char *name, char *value, size_t size)
{
    if (envGet(name, value, size) != 0) {
        errorLog(ERROR_WARNING, "Failed to load constant");
        return -1;
    }
    return 0;
}

static int
loadIntConstant(const char *name, int *value)
{
    char temp[MAX_ENV_VALUE];
    char *endptr;
    long result;

    if (envGet(name, temp, sizeof(temp)) != 0) {
        errorLog(ERROR_WARNING, "Failed to load constant");
        return -1;
    }

    errno = 0;
    result = strtol(temp, &endptr, 10);

    if (errno != 0 || *endptr != '\0' || result > INT_MAX || result < INT_MIN) {
        errorLog(ERROR_WARNING, "Invalid integer constant");
        return -1;
    }

    *value = (int)result;
    return 0;
}

static int
loadLongConstant(const char *name, long *value)
{
    char temp[MAX_ENV_VALUE];
    char *endptr;

    if (envGet(name, temp, sizeof(temp)) != 0) {
        errorLog(ERROR_WARNING, "Failed to load constant");
        return -1;
    }

    errno = 0;
    *value = strtol(temp, &endptr, 10);

    if (errno != 0 || *endptr != '\0') {
        errorLog(ERROR_WARNING, "Invalid long constant");
        return -1;
    }

    return 0;
}

static int
loadBoolConstant(const char *name, int *value)
{
    char temp[MAX_ENV_VALUE];

    if (envGet(name, temp, sizeof(temp)) != 0) {
        errorLog(ERROR_WARNING, "Failed to load constant");
        return -1;
    }

    if (strcmp(temp, "true") == 0 || strcmp(temp, "1") == 0) {
        *value = 1;
    } else if (strcmp(temp, "false") == 0 || strcmp(temp, "0") == 0) {
        *value = 0;
    } else {
        errorLog(ERROR_WARNING, "Invalid boolean constant");
        return -1;
    }

    return 0;
}

void constants_cleanup(void)
{
    /* Clear all sensitive data */
    memset(app_name, 0, sizeof(app_name));
    memset(app_version, 0, sizeof(app_version));
    memset(app_env, 0, sizeof(app_env));
    memset(app_host, 0, sizeof(app_host));
    memset(log_level, 0, sizeof(log_level));
    memset(log_path, 0, sizeof(log_path));
    memset(log_format, 0, sizeof(log_format));

    app_debug = 0;
    app_port = 0;
    log_max_size = 0;
}
