/**
 * \file http_response.h
 * \brief Declares functions for handling HTTP requests and responses.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stdbool.h>

#define RESPONSE_BUFFER_SIZE 16384

void *handle_client(void *client_fd_ptr);

#endif /* HTTP_RESPONSE_H */
