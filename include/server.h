/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#ifndef SERVER_H
#define SERVER_H

#include <openssl/ssl.h>

struct QuicContext;  /* Forward declaration */

/**
 * Server configuration structure
 */
struct ServerConfig
{
    int port;               /* Server listening port */
    const char *ssl_cert;   /* Path to SSL certificate */
    const char *ssl_key;    /* Path to SSL private key */
    const char *db_file;    /* Database file path */
    const char *env_file;   /* Environment file path */
    int max_streams;        /* Maximum number of QUIC streams */
    int quic_timeout;       /* QUIC timeout value in milliseconds */
};

/**
 * Server state structure
 */
struct ServerState
{
    SSL_CTX *ssl_ctx;              /* OpenSSL context */
    struct QuicContext *quic_ctx;  /* QUIC context */
};

extern struct ServerState server_state;

/**
 * Initialize server with given configuration
 * @param config Server configuration structure
 * @return 0 on success, -1 on error
 */
int initServer(const struct ServerConfig *config);

/**
 * Start server main loop
 * @return 0 on success, -1 on error
 */
int runServer(void);

/**
 * Stop the server and cleanup resources including SSL
 */
void stopServer(void);

#endif /* SERVER_H */
