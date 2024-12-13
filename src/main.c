/* filepath: src/main.c */
/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

/* System headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <locale.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <openssl/ssl.h>
#include <limits.h>
#include <time.h>

/* Project headers */
#include "../include/config.h"
#include "../include/server.h"
#include "../include/logger.h"
#include "../include/database.h"
#include "../include/auth.h"
#include "../include/audit.h"
#include "../include/session.h"
#include "../include/utils.h"
#include "../include/data_structures.h"

/* Local definitions */
#define DEFAULT_LOG_LEVEL 0
#define HTTP_INTERNAL_ERROR 500
/* Default paths */
#define DEFAULT_CONFIG_PATH "./etc/env/.env"
#define DEFAULT_LOG_PATH "./var/log/app.log"
#define DEFAULT_DB_PATH "../var/lib/records.rec"
#define DEFAULT_AUTH_PATH "./etc/auth/passwd"
#define DEFAULT_SSL_CERT "./etc/ssl/cert.pem"
#define DEFAULT_SSL_KEY "./etc/ssl/privkey.pem"
#define DEFAULT_AUDIT_PATH "./var/log/audit.log"
#define ERROR_MSG_MAX 256
#define TIMESTAMP_MAX 32

/* Global variables */
volatile sig_atomic_t running = 1;
struct DBHandle db_handle;
/* struct Config *g_config = NULL; */  /* Delete or comment out this line */

/* Error codes */
enum SystemErrorCode {
    ERR_SUCCESS = 0,
    ERR_INVALID_CONFIG = -1,
    ERR_LOG_PATH_TOO_LONG = -2,
    ERR_CREATE_LOG_DIR = -3,
    ERR_INIT_LOGGING = -4,
    ERR_INIT_DATABASE = -5,
    ERR_INIT_SESSION = -6,
    ERR_INIT_AUDIT = -7,
    ERR_INIT_SERVER = -8
};

/* Error tracking structure */
struct SystemState {
    int logging_initialized;
    int database_initialized;
    int session_initialized;
    int audit_initialized;
};

/* Error detail structure */
struct ErrorDetail
{
    int code;
    char message[ERROR_MSG_MAX];
    const char *file;
    int line;
    int sys_errno;
    char timestamp[TIMESTAMP_MAX];
};

/* Forward declarations */
static void cleanupHandler(int signum);
int otherInit(void);
static int initializeSubsystems(const struct Config *config);
struct Config *configInit(const char *path);
void configCleanup(struct Config *config);
int serverStart(const struct ServerConfig *config);

/* Helper function for error details */
static void logErrorDetail(const char *file, int line, const char *fmt, ...)
{
    struct ErrorDetail err;
    va_list args;
    time_t now;
    char *time_str;

    /* Get timestamp */
    now = time(NULL);
    time_str = ctime(&now);
    if (time_str) {
        strncpy(err.timestamp, time_str, TIMESTAMP_MAX - 1);
        err.timestamp[TIMESTAMP_MAX - 1] = '\0';
    }

    /* Store error context */
    err.file = file;
    err.line = line;
    err.sys_errno = errno;

    /* Format message */
    va_start(args, fmt);
    vsnprintf(err.message, ERROR_MSG_MAX - 1, fmt, args);
    va_end(args);

    /* Log to both stderr and log file */
    fprintf(stderr, "[%s] %s:%d: %s (errno: %d - %s)\n",
            err.timestamp, err.file, err.line, err.message,
            err.sys_errno, strerror(err.sys_errno));

    /* Call logError without checking return value since it's void */
    logError("[%s] %s:%d: %s (errno: %d - %s)",
             err.timestamp, err.file, err.line, err.message,
             err.sys_errno, strerror(err.sys_errno));
}

int main(void)
{
    struct Config config;
    int ret;

    memset(&config, 0, sizeof(config));
    strncpy(config.base_path, "..", sizeof(config.base_path) - 1);

    ret = parseEnvFile("../etc/env/.env", &config);
    if (ret != 0) {
        fprintf(stderr, "Failed to load configuration\n");
        return EXIT_FAILURE;
    }

    struct ServerConfig server_config;
    int status;
    struct sigaction sa;

    /* Setup signal handler */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = cleanupHandler;
    if (sigaction(SIGINT, &sa, NULL) == -1 ||
        sigaction(SIGTERM, &sa, NULL) == -1) {
        fprintf(stderr, "Failed to setup signal handlers: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    /* Validate directory structure */
    if (validateRequiredPaths() != 0) {
        fprintf(stderr, "Required directory structure not found\n");
        return EXIT_FAILURE;
    }

    /* Initialize config */
    g_config = configInit(DEFAULT_CONFIG_PATH);
    if (!g_config) {
        fprintf(stderr, "Failed to initialize configuration: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    /* Initialize subsystems */
    status = initializeSubsystems(g_config);
    if (status != ERR_SUCCESS) {
        fprintf(stderr, "Subsystem initialization failed with code: %d\n", status);
        configCleanup(g_config);
        return EXIT_FAILURE;
    }

    /* Initialize server config */
    memset(&server_config, 0, sizeof(server_config));
    server_config.port = g_config->server_port;
    server_config.ssl_cert = g_config->ssl_cert_file;
    server_config.ssl_key = g_config->ssl_key_file;
    server_config.db_file = g_config->db_file;
    server_config.max_streams = g_config->quic_max_streams;
    server_config.quic_timeout = g_config->quic_timeout_ms;

    /* Start server */
    status = serverStart(&server_config);
    if (status != ERR_SUCCESS) {
        fprintf(stderr, "Server failed to start with error code: %d\n", status);
        logError("Server initialization failed");
        configCleanup(g_config);
        return EXIT_FAILURE;
    }

    /* Cleanup */
    configCleanup(g_config);
    return EXIT_SUCCESS;
}

static void cleanupHandler(int signum)
{
    (void)signum; /* Suppress unused parameter warning */
    running = 0;
}

static int initializeSubsystems(const struct Config *config)
{
    struct Response *response;
    const char *log_path;
    char log_dir[PATH_MAX];
    char *last_slash;
    size_t path_len;
    struct SystemState state = {0};
    int status = ERR_SUCCESS;

    if (!config) {
        logErrorDetail(__FILE__, __LINE__, "Invalid configuration");
        return ERR_INVALID_CONFIG;
    }

    /* Use config log path if available, otherwise use default */
    log_path = config->log_path[0] ? config->log_path : DEFAULT_LOG_PATH;

    /* Extract directory path safely */
    path_len = strlen(log_path);
    if (path_len >= PATH_MAX) {
        logErrorDetail(__FILE__, __LINE__, "Log path too long: %s", log_path);
        return ERR_LOG_PATH_TOO_LONG;
    }

    /* Copy path for directory manipulation */
    strncpy(log_dir, log_path, sizeof(log_dir) - 1);
    log_dir[sizeof(log_dir) - 1] = '\0';

    last_slash = strrchr(log_dir, '/');
    if (last_slash) {
        *last_slash = '\0';
        if (createDirectoryIfNotExists(log_dir) != 0) {
            logErrorDetail(__FILE__, __LINE__,
                         "Failed to create log directory: %s", log_dir);
            return ERR_CREATE_LOG_DIR;
        }
    }

    /* Initialize logging with proper path */
    if (logInit(log_path, DEFAULT_LOG_LEVEL) != 0) {
        logErrorDetail(__FILE__, __LINE__,
                      "Failed to initialize logging at: %s", log_path);
        return ERR_INIT_LOGGING;
    }
    state.logging_initialized = 1;

    /* Log paths being used */
    logInfo("Initializing with paths:");
    logInfo("Config: %s", DEFAULT_CONFIG_PATH);
    logInfo("Database: %s", config->db_path);
    logInfo("Log: %s", log_path);
    logInfo("Audit: %s", DEFAULT_AUDIT_PATH);
    logInfo("SSL Cert: %s", config->ssl_cert_file);
    logInfo("SSL Key: %s", config->ssl_key_file);

    /* Validate critical paths exist */
    if (!fileExists(config->db_path)) {
        logErrorDetail(__FILE__, __LINE__,
            "Database file not found at: %s", config->db_path);
        return ERR_INIT_DATABASE;
    }

    /* Initialize database */
    if (dbInit(config->db_path, &db_handle) != 0) {
        logErrorDetail(__FILE__, __LINE__,
                      "Failed to initialize database at: %s", config->db_path);
        status = ERR_INIT_DATABASE;
        goto cleanup;
    }
    state.database_initialized = 1;

    /* Initialize session management */
    if (sessionInit() != 0) {
        logErrorDetail(__FILE__, __LINE__, "Failed to initialize session management");
        status = ERR_INIT_SESSION;
        goto cleanup;
    }
    state.session_initialized = 1;

    /* Initialize audit subsystem */
    if (auditInit(DEFAULT_AUDIT_PATH) != 0) {
        logErrorDetail(__FILE__, __LINE__, "Failed to initialize audit subsystem");
        status = ERR_INIT_AUDIT;
        goto cleanup;
    }
    state.audit_initialized = 1;

    return ERR_SUCCESS;

cleanup:
    if (state.audit_initialized) {
        auditCleanup();
    }
    if (state.session_initialized) {
        sessionCleanup();
    }
    if (state.database_initialized) {
        dbCleanup(&db_handle);
    }
    if (state.logging_initialized) {
        logCleanup();
    }

    /* Create error response if needed */
    if (status != ERR_SUCCESS) {
        response = dsCreateResponse(HTTP_INTERNAL_ERROR);
        if (response) {
            dsFreeResponse(response);
        }
    }

    return status;
}

struct Config *configInit(const char *path)
{
    struct Config *config;

    config = (struct Config *)malloc(sizeof(struct Config));
    if (!config) {
        logErrorDetail(__FILE__, __LINE__,
                      "Failed to allocate config structure");
        return NULL;
    }

    memset(config, 0, sizeof(struct Config));

    /* Load configuration from file */
    if (loadConfig(path, config) != 0) {
        logErrorDetail(__FILE__, __LINE__,
                      "Failed to load configuration from %s", path);
        free(config);
        return NULL;
    }

    return config;
}

void configCleanup(struct Config *config)
{
    if (config) {
        /* Clean up any allocated resources */
        free(config);
    }
}

int otherInit(void)
{
    /* Initialize any other required subsystems */
    if (sessionInit() != 0) {
        logError("Failed to initialize session management");
        return -1;
    }

    /* Initialize audit subsystem */
    if (auditInit(DEFAULT_AUDIT_PATH) != 0) {
        logError("Failed to initialize audit subsystem");
        return -1;
    }

    return 0;
}

int serverStart(const struct ServerConfig *config)
{
    int status;

    if (!config) {
        logError("Invalid server configuration");
        return -1;
    }

    /* Initialize server */
    status = initServer(config);
    if (status != 0) {
        logError("Failed to initialize server");
        return -1;
    }

    /* Run server main loop */
    status = runServer();
    if (status != 0) {
        logError("Server operation failed");
        return -1;
    }

    /* Server stopped, clean up */
    stopServer();
    return 0;
}
