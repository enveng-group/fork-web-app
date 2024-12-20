/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: src/web_server.h */
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

/* System headers */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

/* Constants */
#define MAX_METHOD_LEN 16
#define MAX_URI_LEN 1024
#define MAX_PATH_LEN 1024
#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 8080
#define WWW_ROOT "www"
#define SERVER_STRING "Server: EnvEngWebServer/1.0\r\n"
#define AUDIT_LOG_PATH "var/log/audit.log"
#define MAX_AUDIT_MSG 1024
#define MAX_LOG_SIZE (1024 * 1024)  /* 1MB */
#define MAX_LOG_FILES 5
#define LOG_DIR "var/log"
#define AUDIT_LOG_PATTERN "var/log/audit.log.%d"
#define MIME_HTML "text/html"
#define MIME_PLAIN "text/plain"
#define MIME_CSS "text/css"
#define MIME_JS "text/javascript"
#define MIME_DEFAULT "application/octet-stream"
#define MAX_COOKIE_LEN 1024
#define SESSION_TIMEOUT 3600  /* 1 hour in seconds */

/* Function prototypes */
int setup_server(int port);
int handle_client(int client_socket, const char *root_dir);
ssize_t serve_file(int client_socket, const char *uri);
int validate_email(const char *email);
int validate_project(const char *project);
int set_session_cookie(int client_socket, const char *username);
int validate_session_cookie(const char *cookie_header);

#endif /* WEB_SERVER_H */
