/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>
#include <sys/types.h>
#include <openssl/ssl.h>

/* Buffer size constants */
#define HTTP_MAX_HEADERS 50
#define HTTP_MAX_HEADER_KEY 128
#define HTTP_MAX_HEADER_VALUE 256
#define HTTP_MAX_URL 512
#define HTTP_MAX_BODY 2048
#define HTTP_DEFAULT_STATUS 200

/* HTTP Methods */
enum HttpMethod
{
    HTTP_UNKNOWN = 0,
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_OPTIONS  /* Add this line */
};

/* HTTP Status Codes */
enum HttpStatus
{
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_INTERNAL_ERROR = 500
};

/* HTTP Header structure */
struct HttpHeader
{
    char key[HTTP_MAX_HEADER_KEY];
    char value[HTTP_MAX_HEADER_VALUE];
};

/* HTTP Request structure */
struct HttpRequest
{
    enum HttpMethod method;
    char url[HTTP_MAX_URL];
    struct HttpHeader headers[HTTP_MAX_HEADERS];
    char *body;
    size_t body_len;
};

/* HTTP Response structure */
struct HttpResponse
{
    int status;
    struct HttpHeader headers[HTTP_MAX_HEADERS];
    char *body;
    size_t body_len;
};

/* Function declarations */

/* Initialization */
void httpInit(SSL_CTX *ssl_ctx);

/* Request handling */
int httpParseRequest(const char *raw_request, size_t len, struct HttpRequest *request);
void httpFreeRequest(struct HttpRequest *request);

/* HTTP request handling functions */
int httpHandleConnection(SSL *ssl);
struct HttpResponse* httpCreateResponse(int status_code, const char *content_type);
void httpFreeResponse(struct HttpResponse *response);
int httpSendResponse(SSL *ssl, const struct HttpResponse *response);

/* Response handling */
struct HttpResponse *httpCreateResponse(int status, const char *content_type);
int httpSendResponse(SSL *ssl, const struct HttpResponse *response);
void httpFreeResponse(struct HttpResponse *response);

/* Utility functions */
const char *httpStatusString(int status);

#endif /* HTTP_H */
