/**
 * \file static_file_handler.c
 * \brief Implements functions for handling static file requests.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/static_file_handler.h"
#include "../include/logger.h"
#include "../include/constants.h"
#include "../include/config_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <strings.h>  /* Include for strcasecmp */
#include <sys/socket.h>  /* Include for send */
#include <fcntl.h>  /* Include for open and O_RDONLY */

/* External config variable */
extern Config config;

/**
 * \brief Converts a hexadecimal character to its integer value.
 *
 * \param c The hexadecimal character.
 * \return The integer value.
 */
static int hexValue(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return -1;
}

const char *getMimeType(const char *file_ext)
{
    if (strcasecmp(file_ext, "html") == 0) return "text/html";
    if (strcasecmp(file_ext, "txt") == 0) return "text/plain";
    if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0) return "image/jpeg";
    if (strcasecmp(file_ext, "png") == 0) return "image/png";
    return "application/octet-stream";
}

char *getFileExtension(const char *file_name)
{
    char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name) return "";
    return dot + 1;
}

char *urlDecode(const char *src)
{
    char *decoded = malloc(strlen(src) + 1);
    char *p = decoded;

    if (decoded == NULL)
    {
        return NULL;
    }

    while (*src)
    {
        if (*src == '%')
        {
            if (src[1] && src[2])
            {
                *p++ = (char)((hexValue(src[1]) << 4) | hexValue(src[2]));
                src += 2;
            }
        }
        else if (*src == '+')
        {
            *p++ = ' ';
        }
        else
        {
            *p++ = *src;
        }
        src++;
    }
    *p = '\0';
    return decoded;
}

void serveStaticFile(int client_fd, const char *file_path)
{
    char full_path[4096];  /* Fixed size array */
    struct stat st;
    int file_fd;
    char *file_ext;
    const char *mime_type;
    char header[256];
    char buffer[1024];
    ssize_t bytes_read;

    snprintf(full_path, sizeof(full_path), "%s/%s", config.document_root, file_path);

    if (stat(full_path, &st) == -1)
    {
        const char *not_found_response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_fd, not_found_response, strlen(not_found_response), 0);
        return;
    }

    file_ext = getFileExtension(full_path);
    mime_type = getMimeType(file_ext);
    if (strcmp(mime_type, "application/octet-stream") == 0)
    {
        const char *unsupported_response = "HTTP/1.1 415 Unsupported Media Type\r\n\r\n";
        send(client_fd, unsupported_response, strlen(unsupported_response), 0);
        return;
    }

    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", mime_type, st.st_size);
    send(client_fd, header, strlen(header), 0);

    /* Send the file content */
    file_fd = open(full_path, O_RDONLY);
    if (file_fd == -1)
    {
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        send(client_fd, error_response, strlen(error_response), 0);
        return;
    }

    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0)
    {
        send(client_fd, buffer, bytes_read, 0);
    }

    close(file_fd);
}
