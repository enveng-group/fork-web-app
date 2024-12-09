/**
 * \file http_response.c
 * \brief Implements HTTP response functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/http_response.h"
#include "../include/logger.h"
#include "../include/http_parser.h"
#include "../include/constants.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <sys/socket.h>
#include <strings.h>  /* Include for strcasecmp */

/**
 * \brief Gets the MIME type based on the file extension.
 *
 * \param file_ext The file extension.
 * \return The MIME type.
 */

/**
 * \brief Gets the file extension from the file name.
 *
 * \param file_name The file name.
 * \return The file extension.
 */
char *get_file_extension(const char *file_name)
{
    const char *dot = strrchr(file_name, '.');
    return (dot && dot != file_name) ? strdup(dot + 1) : strdup("");
}

/**
 * \brief Decodes a URL-encoded string.
 *
 * \param src The URL-encoded string.
 * \return The decoded string.
 */
char *url_decode(const char *src)
{
    char *decoded = malloc(strlen(src) + 1);
    char *p = decoded;

    if (decoded == NULL)
    {
        return NULL;
    }

    while (*src)
    {
        if (*src == '%' && isxdigit(*(src + 1)) && isxdigit(*(src + 2)))
        {
            char hex[3];
            hex[0] = *(src + 1);
            hex[1] = *(src + 2);
            hex[2] = '\0';
            *p++ = (char)strtol(hex, NULL, 16);
            src += 3;
        }
        else
        {
            *p++ = *src++;
        }
    }
    *p = '\0';
    return decoded;
}

/**
 * \brief Sends an HTTP response.
 *
 * \param client_fd The client file descriptor.
 * \param file_path The file path.
 */
void send_response(int client_fd, const char *file_path)
{
    char full_path[PATH_MAX];
    int file_fd;
    char *file_extension;
    const char *mime_type;
    char header[256];
    char buffer[RESPONSE_BUFFER_SIZE];
    ssize_t bytes_read;

    snprintf(full_path, sizeof(full_path), "var/www/html/%s", file_path);

    file_fd = open(full_path, O_RDONLY);
    if (file_fd == -1)
    {
        const char *not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(client_fd, not_found_response, strlen(not_found_response), 0);
        return;
    }

    /* Get the file extension and MIME type */
    file_extension = get_file_extension(file_path);
    mime_type = (file_extension);
    free(file_extension);

    if (strcmp(mime_type, "application/octet-stream") == 0)
    {
        const char *unsupported_response = "HTTP/1.1 415 Unsupported Media Type\r\nContent-Length: 0\r\n\r\n";
        send(client_fd, unsupported_response, strlen(unsupported_response), 0);
        close(file_fd);
        return;
    }

    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", mime_type);
    send(client_fd, header, strlen(header), 0);

    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        send(client_fd, buffer, bytes_read, 0);
    }

    close(file_fd);
}

void *handle_client(void *client_fd_ptr)
{
    int client_fd = *(int *)client_fd_ptr;
    free(client_fd_ptr);

    char buffer[RESPONSE_BUFFER_SIZE];
    ssize_t bytes_read;
    char file_path[256];

    bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
        logInfo("Received request: %s", buffer);

        parse_http_request(buffer, file_path);
        route_request(client_fd, file_path);
    }

    close(client_fd);
    return NULL;
}
