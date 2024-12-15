/* filepath: /devcontainer/web-app/include/constants.h */
/* Copyright 2024 Enveng Group */
#ifndef CONSTANTS_H
#define CONSTANTS_H

/* System headers */
#include <stdio.h>
#include <stdlib.h>

/* System paths */
#define DEFAULT_CONFIG_PATH "/etc/config"
#define DEFAULT_LOG_PATH "/var/log/app.log"  /* Updated to be more specific */
#define DEFAULT_ENV_PATH "/etc/config/env"
#define DEFAULT_LOG_DIR "/var/log"

/* Configuration limits */
#define MAX_ENV_NAME 64
#define MAX_ENV_VALUE 1024
#define MAX_ENV_KEY 256
#define MAX_LINE_LENGTH (MAX_ENV_KEY + MAX_ENV_VALUE + 2)
#define MAX_CONFIG_LINE 2048
#define MAX_LOG_SIZE (10 * 1024 * 1024) /* 10MB */

/* Exit codes */
#define EXIT_CONFIG_ERROR 78 /* EX_CONFIG from sysexits.h */
#define EXIT_ENV_ERROR 71    /* EX_OSERR from sysexits.h */

/* Return codes */
#define SUCCESS 0
#define ERROR -1
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* App constants */
#define APP_NAME "TestApp"
#define APP_VERSION "1.0.0"
#define APP_ENV "test"
#define APP_DEBUG 1
#define APP_PORT 8080
#define APP_HOST "localhost"

/* Log constants */
#define LOG_LEVEL "debug"
#define LOG_PATH "/var/log"
#define LOG_FORMAT "json"
#define LOG_MAX_SIZE 1048576

/* Function prototypes */
int constants_init(void);
void constants_cleanup(void);
const char *get_app_name(void);
const char *get_app_version(void);
const char *get_app_env(void);
int get_app_debug(void);
int get_app_port(void);
const char *get_app_host(void);
const char *get_log_level(void);
const char *get_log_path(void);
const char *get_log_format(void);
long get_log_max_size(void);

#endif /* CONSTANTS_H */
