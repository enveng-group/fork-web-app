/*
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
/* filepath: src/web_server.c */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include "../include/web_server.h"

static int
parse_request_line(FILE *client_stream, char *method, char *uri)
{
    char line[MAX_BUFFER_SIZE];
    char *token;
    char *saveptr;

    if (fgets(line, sizeof(line), client_stream) == NULL) {
        return -1;
    }

    /* Parse method */
    token = strtok_r(line, " ", &saveptr);
    if (token == NULL) {
        return -1;
    }
    strncpy(method, token, MAX_METHOD_LEN - 1);
    method[MAX_METHOD_LEN - 1] = '\0';

    /* Parse URI */
    token = strtok_r(NULL, " ", &saveptr);
    if (token == NULL) {
        return -1;
    }
    strncpy(uri, token, MAX_URI_LEN - 1);
    uri[MAX_URI_LEN - 1] = '\0';

    return 0;
}

ssize_t
serve_file(int client_socket, const char *uri)
{
    char filepath[MAX_PATH_LEN];
    char buffer[MAX_BUFFER_SIZE];
    FILE *file;
    struct stat st;
    size_t bytes_read;
    ssize_t total_bytes;
    ssize_t write_result;
    const char *clean_uri;
    char *query_pos;
    char uri_copy[MAX_URI_LEN];

    total_bytes = 0;

    /* Make a copy of URI for manipulation */
    strncpy(uri_copy, uri, MAX_URI_LEN - 1);
    uri_copy[MAX_URI_LEN - 1] = '\0';

    /* Remove query string if present */
    query_pos = strchr(uri_copy, '?');
    if (query_pos != NULL) {
        *query_pos = '\0';
    }

    /* Basic URI sanitization */
    clean_uri = (*uri_copy == '/') ? uri_copy + 1 : uri_copy;
    if (*clean_uri == '\0') {
        clean_uri = "index.html";
    }

    /* Construct file path */
    snprintf(filepath, sizeof(filepath), "%s/%s", WWW_ROOT, clean_uri);

    /* Check if file exists */
    if (stat(filepath, &st) < 0) {
        dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
        return -1;
    }

    /* Rest of the function remains the same */
    file = fopen(filepath, "rb");
    if (file == NULL) {
        dprintf(client_socket, "HTTP/1.0 500 Internal Error\r\n\r\n");
        return -1;
    }

    /* Send headers */
    dprintf(client_socket, "HTTP/1.0 200 OK\r\n%sContent-Type: text/html\r\n\r\n",
            SERVER_STRING);

    /* Send file content */
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write_result = write(client_socket, buffer, bytes_read);
        if (write_result < 0) {
            fclose(file);
            return -1;
        }
        total_bytes += write_result;
        if (total_bytes < 0) { /* Check for overflow */
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return total_bytes;
}

int
setup_server(int port)
{
    struct sockaddr_in server_addr;
    int sock_fd;
    int optval = 1;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        return -1;
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((uint16_t)port);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sock_fd);
        return -1;
    }

    if (listen(sock_fd, 10) < 0) {
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

int
handle_client(int client_socket, const char *root_dir)
{
    char method[MAX_METHOD_LEN];
    char uri[MAX_URI_LEN];
    FILE *client_stream;
    int result = -1;

    (void)root_dir;

    client_stream = fdopen(client_socket, "r");
    if (client_stream == NULL) {
        return -1;
    }

    if (parse_request_line(client_stream, method, uri) < 0) {
        fclose(client_stream);
        return -1;
    }

    if (strcmp(method, "GET") == 0) {
        result = (int)serve_file(client_socket, uri);
    }

    fclose(client_stream);
    return result;
}
