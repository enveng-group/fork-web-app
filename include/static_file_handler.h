/**
 * \file static_file_handler.h
 * \brief Header file for handling static file requests.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef STATIC_FILE_HANDLER_H
#define STATIC_FILE_HANDLER_H

#include <stdbool.h>

#define RESPONSE_BUFFER_SIZE 16384

void serveStaticFile(int client_fd, const char *file_path);
char *getFileExtension(const char *file_name);
const char *getMimeType(const char *file_ext);
char *urlDecode(const char *src);

#endif /* STATIC_FILE_HANDLER_H */
