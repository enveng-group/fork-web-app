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

/* Function prototypes */
int setup_server(int port);
int handle_client(int client_socket, const char *root_dir);
ssize_t serve_file(int client_socket, const char *uri);

#endif /* WEB_SERVER_H */
