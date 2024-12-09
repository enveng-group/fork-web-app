/**
 * \file http_parser.c
 * \brief Implements functions for parsing HTTP requests.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/http_parser.h"
#include "../include/static_file_handler.h"
#include <stdio.h>
#include <string.h>

void parse_http_request(const char *request, char *file_path)
{
    sscanf(request, "GET /%255s HTTP/1.1", file_path);
}

void route_request(int client_fd, const char *file_path)
{
    if (file_path == NULL)
    {
        fprintf(stderr, "Error: file_path is NULL\n");
        return;
    }

    printf("Routing request for file: %s\n", file_path);

    if (strcmp(file_path, "") == 0)
    {
        file_path = "index.html";
    }

    serveStaticFile(client_fd, file_path);
}
