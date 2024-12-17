/* filepath: /devcontainer/web-app/include/constants.h */
/* Copyright 2024 Enveng Group */
#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Log configuration - keep only one definition of each constant */
#define LOG_MAX_SIZE (1024 * 1024)     /* 1MB */
#define LOG_MAX_FILES 10
#define LOG_MAX_AGE (24 * 60 * 60)      /* 24 hours */

/* Dependencies */
#include <sys/types.h>
#include "common.h"

/* System paths */
#define DEFAULT_CONFIG_PATH "test/test.conf"
#define DEFAULT_LOG_PATH "test/test.log"
#define DEFAULT_ENV_PATH "test/test.env"
#define DEFAULT_LOG_DIR "/var/log"

/* Configuration limits */
#define MAX_ENV_NAME 64
#define MAX_ENV_VALUE 1024
#define MAX_ENV_KEY 256
#define MAX_LINE_LENGTH (MAX_ENV_KEY + MAX_ENV_VALUE + 2)
#define MAX_CONFIG_LINE 2048
#define MAX_CMD_LEN 1024      /* From shell.h */
#define MAX_PROCESSES 64      /* From process.h */
#define CACHE_MAX_ENTRIES 1024 /* From cache.h */

/* Exit codes */
#define EXIT_CONFIG_ERROR 78 /* EX_CONFIG from sysexits.h */
#define EXIT_ENV_ERROR 71    /* EX_OSERR from sysexits.h */

/* App constants */
#define APP_NAME "TestApp"
#define APP_VERSION "1.0.0"
#define APP_ENV "test"
#define APP_DEBUG 1
#define APP_PORT 8080
#define APP_HOST "localhost"

/* Log configuration */
#define LOG_LEVEL "debug"
#define LOG_PATH "/var/log"
#define LOG_FORMAT "text"
#define DEFAULT_LOG_MODE 0644
#define DEFAULT_LOG_FORMAT "text"
#define DEFAULT_CACHE_DRIVER "memory"

/* Function prototypes */
int constants_init(void);
void constants_cleanup(void);

/* Getter function prototypes */
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

/* Cache setting getters */
const char *get_cache_driver(void);
int get_cache_ttl(void);

#endif /* CONSTANTS_H */
