/* filepath: /devcontainer/web-app/src/main.c */
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

#define DEFAULT_CONFIG_PATH "/etc/env/.env"
#define DEFAULT_LOG_PATH "/etc/log/app.log"
#define DEFAULT_DB_PATH "/etc/db/records.rec"
#define DEFAULT_AUTH_PATH "/etc/auth/passwd"
#define DEFAULT_SSL_CERT "/etc/ssl/cert.pem"
#define DEFAULT_SSL_KEY "/etc/ssl/privkey.pem"
#define DEFAULT_AUDIT_PATH "/var/log/webapp/audit.log"
#define DEFAULT_LOG_LEVEL 0
#define HTTP_INTERNAL_ERROR 500

/* Global variables */
volatile sig_atomic_t running = 1;
struct DBHandle db_handle;
/* struct Config *g_config = NULL; */  /* Delete or comment out this line */

/* Forward declarations */
static void cleanupHandler(int signum);
int otherInit(void);
static int initializeSubsystems(const struct Config *config);
struct Config *configInit(const char *path);
void configCleanup(struct Config *config);
int serverStart(const struct ServerConfig *config);

int main(void)
{
    struct ServerConfig server_config;
    int status;
    struct sigaction sa;

    /* Setup signal handler */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = cleanupHandler;
    if (sigaction(SIGINT, &sa, NULL) == -1 ||
        sigaction(SIGTERM, &sa, NULL) == -1) {
        fprintf(stderr, "Failed to setup signal handlers\n");
        return EXIT_FAILURE;
    }

    /* Initialize config */
    g_config = configInit(DEFAULT_CONFIG_PATH);
    if (!g_config) {
        fprintf(stderr, "Failed to initialize configuration\n");
        return EXIT_FAILURE;
    }

    /* Initialize subsystems */
    status = initializeSubsystems(g_config);
    if (status != 0) {
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
    if (status != 0) {
        fprintf(stderr, "Server failed to start\n");
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

    /* Initialize logging */
    if (logInit(config->log_path, DEFAULT_LOG_LEVEL) != 0) {
        fprintf(stderr, "Failed to initialize logging\n");
        return -1;
    }

    /* Initialize database */
    if (dbInit(config->db_path, &db_handle) != 0) {
        fprintf(stderr, "Failed to initialize database\n");
        return -1;
    }

    /* Initialize other subsystems */
    if (otherInit() != 0) {
        response = dsCreateResponse(HTTP_INTERNAL_ERROR);
        if (response) {
            dsFreeResponse(response);
        }
        return -1;
    }

    return 0;
}

struct Config *configInit(const char *path)
{
    struct Config *config;

    config = (struct Config *)malloc(sizeof(struct Config));
    if (!config) {
        fprintf(stderr, "Failed to allocate config structure\n");
        return NULL;
    }

    memset(config, 0, sizeof(struct Config));

    /* Load configuration from file */
    if (loadConfig(path, config) != 0) {
        fprintf(stderr, "Failed to load configuration from %s\n", path);
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
