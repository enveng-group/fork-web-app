/* include/data_structures.h */
/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h>

/* Maximum sizes for various fields */
#define DS_MAX_HEADERS 50
#define DS_MAX_URL_LEN 2048
#define DS_MAX_USERNAME_LEN 32
#define DS_MAX_PASSWORD_LEN 128
#define DS_MAX_KEY_LEN 128
#define DS_MAX_VALUE_LEN 1024

/**
 * Header structure for storing key-value pairs
 */
struct Header {
    char key[DS_MAX_KEY_LEN];
    char value[DS_MAX_VALUE_LEN];
};

/**
 * Request structure for handling web requests
 */
struct Request {
    struct Header headers[DS_MAX_HEADERS];
    size_t header_count;
    char url[DS_MAX_URL_LEN];
    char *body;
    size_t body_length;
    int method;  /* HTTP method (GET, POST, etc.) */
};

/**
 * Response structure for handling web responses
 */
struct Response {
    int status_code;
    struct Header headers[DS_MAX_HEADERS];
    size_t header_count;
    char *body;
    size_t body_length;
};

/**
 * UserCredentials structure for authentication
 */
struct UserCredentials {
    char username[DS_MAX_USERNAME_LEN];
    char password[DS_MAX_PASSWORD_LEN];
};

/* Function prototypes */

/**
 * Initialize a new request structure
 * @return Pointer to initialized Request structure or NULL on failure
 */
struct Request *dsCreateRequest(void);

/**
 * Initialize a new response structure
 * @param status_code Initial status code
 * @return Pointer to initialized Response structure or NULL on failure
 */
struct Response *dsCreateResponse(int status_code);

/**
 * Add a header to a request
 * @param request Request structure
 * @param key Header key
 * @param value Header value
 * @return 0 on success, -1 on failure
 */
int dsAddRequestHeader(struct Request *request, const char *key, const char *value);

/**
 * Add a header to a response
 * @param response Response structure
 * @param key Header key
 * @param value Header value
 * @return 0 on success, -1 on failure
 */
int dsAddResponseHeader(struct Response *response, const char *key, const char *value);

/**
 * Free a request structure and its contents
 * @param request Request to free
 */
void dsFreeRequest(struct Request *request);

/**
 * Free a response structure and its contents
 * @param response Response to free
 */
void dsFreeResponse(struct Response *response);

/**
 * Create new user credentials
 * @param username Username string
 * @param password Password string
 * @return Pointer to new UserCredentials or NULL on failure
 */
struct UserCredentials *dsCreateUserCredentials(const char *username, const char *password);

/**
 * Free user credentials structure
 * @param credentials UserCredentials to free
 */
void dsFreeUserCredentials(struct UserCredentials *credentials);

#endif /* DATA_STRUCTURES_H */
