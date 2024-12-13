/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* http.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../include/http.h"
#include "../include/logger.h"
#include "../include/session.h"
#include "../include/auth.h"
#include "../include/data_structures.h"
#include "../include/config.h"

static SSL_CTX *ssl_context = NULL;

void httpInit(SSL_CTX *ssl_ctx)
{
    ssl_context = ssl_ctx;
}

static const char *httpMethodToString(enum HttpMethod method)
{
    switch (method)
    {
        case HTTP_GET:     return "GET";
        case HTTP_POST:    return "POST";
        case HTTP_PUT:     return "PUT";
        case HTTP_DELETE:  return "DELETE";
        case HTTP_OPTIONS: return "OPTIONS";
        default:          return "UNKNOWN";
    }
}

static enum HttpMethod httpStringToMethod(const char *method_str)
{
    if (!method_str) return HTTP_UNKNOWN;

    if (strcmp(method_str, "GET") == 0)    return HTTP_GET;
    if (strcmp(method_str, "POST") == 0)   return HTTP_POST;
    if (strcmp(method_str, "PUT") == 0)    return HTTP_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_DELETE;

    return HTTP_UNKNOWN;
}

static int httpAddHeader(struct HttpResponse *response, const char *key, const char *value)
{
    size_t i;

    if (!response || !key || !value) {
        logError("Invalid parameters for httpAddHeader");
        return -1;
    }

    for (i = 0; i < HTTP_MAX_HEADERS; i++) {
        if (!response->headers[i].key[0]) {
            strncpy(response->headers[i].key, key, HTTP_MAX_HEADER_KEY - 1);
            strncpy(response->headers[i].value, value, HTTP_MAX_HEADER_VALUE - 1);
            return 0;
        }
    }

    return -1;
}

static void httpAddDefaultHeaders(struct HttpResponse *response, const struct Config *config)
{
    char date_buf[32];
    time_t now;
    struct tm *tm_info;

    if (!response || !config) {
        logError("Invalid parameters for httpAddDefaultHeaders");
        return;
    }

    /* Get current time */
    time(&now);
    tm_info = gmtime(&now);
    strftime(date_buf, sizeof(date_buf), "%a, %d %b %Y %H:%M:%S GMT", tm_info);

    /* Add standard headers */
    httpAddHeader(response, "Server", "EnvEng/1.0");
    httpAddHeader(response, "Date", date_buf);

    /* Add CORS headers from config */
    if (config->allowed_origins[0]) {
        httpAddHeader(response, "Access-Control-Allow-Origin", config->allowed_origins);
    }
    if (config->allowed_methods[0]) {
        httpAddHeader(response, "Access-Control-Allow-Methods", config->allowed_methods);
    }
    if (config->allowed_headers[0]) {
        httpAddHeader(response, "Access-Control-Allow-Headers", config->allowed_headers);
    }
}

struct HttpResponse *httpCreateResponse(int status, const char *content_type)
{
    struct HttpResponse *response;
    extern struct Config *g_config; /* Add this declaration */

    response = (struct HttpResponse *)malloc(sizeof(struct HttpResponse));
    if (!response) {
        logError("Failed to allocate response structure");
        return NULL;
    }

    memset(response, 0, sizeof(struct HttpResponse));
    response->status = status;

    httpAddDefaultHeaders(response, g_config); /* Pass config */

    if (content_type) {
        httpAddHeader(response, "Content-Type", content_type);
    }

    return response;
}

void httpFreeResponse(struct HttpResponse *response)
{
    if (response) {
        if (response->body) {
            free(response->body);
        }
        free(response);
    }
}

int httpHandleConnection(SSL *ssl)
{
    struct HttpRequest *request;
    struct HttpResponse *response;
    char buffer[4096];
    int bytes;

    if (!ssl) {
        logError("Invalid SSL connection");
        return -1;
    }

    request = (struct HttpRequest *)malloc(sizeof(struct HttpRequest));
    if (!request) {
        logError("Failed to create request structure");
        return -1;
    }

    memset(request, 0, sizeof(struct HttpRequest));

    bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        free(request);
        return -1;
    }
    buffer[bytes] = '\0';

    if (httpParseRequest(buffer, bytes, request) != 0) {
        response = httpCreateResponse(HTTP_BAD_REQUEST, "text/plain");
        if (response) {
            httpAddHeader(response, "Content-Type", "text/plain");
            response->body = strdup("Bad Request");
            response->body_len = strlen(response->body);
            httpSendResponse(ssl, response);
            httpFreeResponse(response);
        }
        free(request);
        return -1;
    }

    logDebug("Received %s request for %s",
            httpMethodToString(request->method), request->url);

    free(request);
    return 0;
}

int httpParseRequest(const char *raw_request, size_t len, struct HttpRequest *request)
{
    char *line;
    char *method_str;
    char *url;
    char *saveptr;
    char *request_copy;

    if (!raw_request || !request || len == 0) {
        return -1;
    }

    request_copy = (char *)malloc(len + 1);
    if (!request_copy) {
        return -1;
    }

    memset(request, 0, sizeof(struct HttpRequest));
    memcpy(request_copy, raw_request, len);
    request_copy[len] = '\0';

    /* Parse first line */
    line = strtok_r(request_copy, "\r\n", &saveptr);
    if (!line) {
        free(request_copy);
        return -1;
    }

    method_str = strtok(line, " ");
    url = strtok(NULL, " ");

    if (!method_str || !url) {
        free(request_copy);
        return -1;
    }

    request->method = httpStringToMethod(method_str);
    strncpy(request->url, url, HTTP_MAX_URL - 1);

    /* Parse headers */
    while ((line = strtok_r(NULL, "\r\n", &saveptr))) {
        char *key;
        char *value;

        if (*line == '\0') break;  /* End of headers */

        key = strtok(line, ":");
        value = strtok(NULL, "");

        if (key && value) {
            /* Trim leading whitespace from value */
            while (*value && isspace(*value)) value++;
            httpAddHeader((struct HttpResponse *)request, key, value);
        }
    }

    free(request_copy);
    return 0;
}

int httpSendResponse(SSL *ssl, const struct HttpResponse *response)
{
    char buffer[1024];
    size_t i;
    int written;

    if (!ssl || !response) {
        return -1;
    }

    /* Write status line */
    snprintf(buffer, sizeof(buffer), "HTTP/1.1 %d %s\r\n",
             response->status, httpStatusString(response->status));

    written = SSL_write(ssl, buffer, strlen(buffer));
    if (written <= 0) {
        logError("SSL write failed: %s", ERR_error_string(ERR_get_error(), NULL));
        return -1;
    }

    /* Write headers */
    for (i = 0; i < HTTP_MAX_HEADERS && response->headers[i].key[0]; i++) {
        snprintf(buffer, sizeof(buffer), "%s: %s\r\n",
                 response->headers[i].key, response->headers[i].value);

        written = SSL_write(ssl, buffer, strlen(buffer));
        if (written <= 0) {
            return -1;
        }
    }

    /* End of headers */
    SSL_write(ssl, "\r\n", 2);

    /* Write body if present */
    if (response->body && response->body_len > 0) {
        written = SSL_write(ssl, response->body, response->body_len);
        if (written <= 0) {
            return -1;
        }
    }

    return 0;
}

const char *httpStatusString(int status)
{
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        default: return "Unknown";
    }
}
