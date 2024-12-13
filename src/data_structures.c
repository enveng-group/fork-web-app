/* src/data_structures.c */
/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/data_structures.h"
#include "../include/logger.h"

struct Request *dsCreateRequest(void)
{
    struct Request *request;

    request = (struct Request *)malloc(sizeof(struct Request));
    if (!request) {
        logError("Failed to allocate request structure");
        return NULL;
    }

    memset(request, 0, sizeof(struct Request));
    return request;
}

struct Response *dsCreateResponse(int status_code)
{
    struct Response *response;

    response = (struct Response *)malloc(sizeof(struct Response));
    if (!response) {
        logError("Failed to allocate response structure");
        return NULL;
    }

    memset(response, 0, sizeof(struct Response));
    response->status_code = status_code;
    return response;
}

int dsAddRequestHeader(struct Request *request, const char *key, const char *value)
{
    if (!request || !key || !value) {
        logError("Invalid parameters for dsAddRequestHeader");
        return -1;
    }

    if (request->header_count >= DS_MAX_HEADERS) {
        logError("Maximum number of headers reached");
        return -1;
    }

    strncpy(request->headers[request->header_count].key, key, DS_MAX_KEY_LEN - 1);
    strncpy(request->headers[request->header_count].value, value, DS_MAX_VALUE_LEN - 1);
    request->header_count++;

    return 0;
}

int dsAddResponseHeader(struct Response *response, const char *key, const char *value)
{
    if (!response || !key || !value) {
        logError("Invalid parameters for dsAddResponseHeader");
        return -1;
    }

    if (response->header_count >= DS_MAX_HEADERS) {
        logError("Maximum number of headers reached");
        return -1;
    }

    strncpy(response->headers[response->header_count].key, key, DS_MAX_KEY_LEN - 1);
    strncpy(response->headers[response->header_count].value, value, DS_MAX_VALUE_LEN - 1);
    response->header_count++;

    return 0;
}

void dsFreeRequest(struct Request *request)
{
    if (!request) {
        return;
    }

    if (request->body) {
        free(request->body);
    }

    free(request);
}

void dsFreeResponse(struct Response *response)
{
    if (!response) {
        return;
    }

    if (response->body) {
        free(response->body);
    }

    free(response);
}

struct UserCredentials *dsCreateUserCredentials(const char *username, const char *password)
{
    struct UserCredentials *credentials;

    if (!username || !password) {
        logError("Invalid parameters for dsCreateUserCredentials");
        return NULL;
    }

    credentials = (struct UserCredentials *)malloc(sizeof(struct UserCredentials));
    if (!credentials) {
        logError("Failed to allocate credentials structure");
        return NULL;
    }

    strncpy(credentials->username, username, DS_MAX_USERNAME_LEN - 1);
    strncpy(credentials->password, password, DS_MAX_PASSWORD_LEN - 1);

    return credentials;
}

void dsFreeUserCredentials(struct UserCredentials *credentials)
{
    if (!credentials) {
        return;
    }

    /* Securely clear sensitive data */
    memset(credentials->password, 0, DS_MAX_PASSWORD_LEN);
    free(credentials);
}
