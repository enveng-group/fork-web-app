/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>
#include <limits.h>  /* For PATH_MAX - POSIX compliant */

#define MAX_ENV_LINE 1024
#define MAX_ENV_VALUE 256

/* Default paths */
#define DEFAULT_CONFIG_PATH "/devcontainer/web-app/etc/env/.env"
#define DEFAULT_LOG_PATH "/devcontainer/web-app/var/log/app.log"
#define DEFAULT_DB_PATH "/devcontainer/web-app/var/lib/records.rec"
#define DEFAULT_AUTH_PATH "/devcontainer/web-app/etc/auth/passwd"
#define DEFAULT_SSL_CERT "/devcontainer/web-app/etc/ssl/cert.pem"
#define DEFAULT_SSL_KEY "/devcontainer/web-app/etc/ssl/privkey.pem"
#define DEFAULT_AUDIT_PATH "/devcontainer/web-app/var/log/audit.log"

struct Config {
    char log_path[PATH_MAX];
    char db_path[PATH_MAX];
    char audit_path[PATH_MAX];
    int server_port;
    char server_host[256];
    char ssl_cert_file[256];
    char ssl_key_file[256];
    char db_file[256];
    char log_file[256];
    char log_level[16];
    int max_connections;
    char locale[32];
    char allowed_origins[256];
    char allowed_methods[256];
    char allowed_headers[256];
    char app_name[64];
    char app_domain[256];
    int quic_max_streams;
    int quic_timeout_ms;
    int quic_mtu_size;
};

/* Add this line for the global config variable */
extern struct Config *g_config;

int loadConfig(const char *env_file, struct Config *config);
void printConfig(const struct Config *config);

#endif /* CONFIG_H */
