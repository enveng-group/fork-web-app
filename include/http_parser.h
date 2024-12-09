/**
 * \file http_parser.h
 * \brief Header file for parsing HTTP requests.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

void parse_http_request(const char *request, char *file_path);
void route_request(int client_fd, const char *file_path);

#endif /* HTTP_PARSER_H */
