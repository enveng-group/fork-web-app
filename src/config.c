/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/config.h"
#include "../include/logger.h"

/* Define the global variable */
struct Config *g_config = NULL;  /* Global configuration instance */

static char* trimWhitespace(char *str)
{
    char *end;

    while((*str == ' ') || (*str == '\t')) str++;

    if(*str == '\0') return str;

    end = str + strlen(str) - 1;
    while(end > str && ((*end == ' ') || (*end == '\t'))) end--;

    *(end + 1) = '\0';

    return str;
}

int loadConfig(const char *env_file, struct Config *config)
{
    FILE *fp;
    char line[MAX_ENV_LINE];
    char *key, *value;

    if (!env_file || !config) {
        logError("Invalid parameters for loadConfig");
        return -1;
    }

    /* Initialize config with defaults */
    memset(config, 0, sizeof(struct Config));
    config->server_port = 8080; /* Default port */
    strncpy(config->server_host, "127.0.0.1", sizeof(config->server_host) - 1);

    fp = fopen(env_file, "r");
    if (!fp) {
        logError("Cannot open config file: %s", env_file);
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n') continue;

        key = strtok(line, "=");
        value = strtok(NULL, "\n");

        if (!key || !value) continue;

        key = trimWhitespace(key);
        value = trimWhitespace(value);

        if (strcmp(key, "SERVER_PORT") == 0) {
            config->server_port = atoi(value);
        } else if (strcmp(key, "SERVER_HOST") == 0) {
            strncpy(config->server_host, value, sizeof(config->server_host) - 1);
        } else if (strcmp(key, "SSL_CERT_FILE") == 0) {
            strncpy(config->ssl_cert_file, value, sizeof(config->ssl_cert_file) - 1);
        } else if (strcmp(key, "SSL_KEY_FILE") == 0) {
            strncpy(config->ssl_key_file, value, sizeof(config->ssl_key_file) - 1);
        } else if (strcmp(key, "DB_FILE") == 0) {
            strncpy(config->db_file, value, sizeof(config->db_file) - 1);
        } else if (strcmp(key, "LOG_FILE") == 0) {
            strncpy(config->log_file, value, sizeof(config->log_file) - 1);
        } else if (strcmp(key, "LOG_LEVEL") == 0) {
            strncpy(config->log_level, value, sizeof(config->log_level) - 1);
        } else if (strcmp(key, "QUIC_MAX_STREAMS") == 0) {
            config->quic_max_streams = atoi(value);
        } else if (strcmp(key, "QUIC_TIMEOUT_MS") == 0) {
            config->quic_timeout_ms = atoi(value);
        } else if (strcmp(key, "QUIC_MTU_SIZE") == 0) {
            config->quic_mtu_size = atoi(value);
        }
    }

    fclose(fp);

    /* Set default audit path if not specified */
    if (getenv("AUDIT_PATH") != NULL) {
        strncpy(config->audit_path, getenv("AUDIT_PATH"), PATH_MAX - 1);
    } else {
        strncpy(config->audit_path, DEFAULT_AUDIT_PATH, PATH_MAX - 1);
    }
    config->audit_path[PATH_MAX - 1] = '\0';

    /* Validate required configurations */
    if (!config->server_port || !config->ssl_cert_file[0] || !config->ssl_key_file[0]) {
        logError("Missing required configuration parameters");
        return -1;
    }

    if (config->quic_max_streams <= 0 || config->quic_max_streams > 100) {
        logError("Invalid QUIC_MAX_STREAMS value");
        return -1;
    }
    if (config->quic_timeout_ms < 1000 || config->quic_timeout_ms > 60000) {
        logError("Invalid QUIC_TIMEOUT_MS value");
        return -1;
    }

    return 0;
}

void printConfig(const struct Config *config)
{
    logInfo("Configuration:");
    logInfo("Server Port: %d", config->server_port);
    logInfo("Server Host: %s", config->server_host);
    logInfo("SSL Cert: %s", config->ssl_cert_file);
    logInfo("SSL Key: %s", config->ssl_key_file);
    logInfo("Database: %s", config->db_file);
    logInfo("Log File: %s", config->log_file);
    logInfo("Log Level: %s", config->log_level);
    logInfo("Max Connections: %d", config->max_connections);
    logInfo("Locale: %s", config->locale);
    logInfo("QUIC Max Streams: %d", config->quic_max_streams);
    logInfo("QUIC Timeout (ms): %d", config->quic_timeout_ms);
}
