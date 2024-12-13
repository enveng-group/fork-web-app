/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* include/quic_module.h */
#ifndef QUIC_MODULE_H
#define QUIC_MODULE_H

#include <sys/socket.h>
#include <stddef.h>

#define QUIC_MAX_STREAMS 10
#define QUIC_DEFAULT_TIMEOUT_MS 30000
#define QUIC_MAX_DATA_LENGTH 1452 /* QUIC MTU size */

/* QUIC context structure */
struct QuicContext {
    int socket_fd;
    int max_streams;
    int timeout_ms;
    void *ssl_context;
    unsigned char *send_buffer;
    unsigned char *recv_buffer;
    size_t buffer_size;
    int is_initialized;
};

/* Function declarations */
struct QuicContext* initializeQuicContext(int socket_fd, void *ssl_ctx,
                                        int max_streams, int timeout_ms);
void cleanupQuicContext(struct QuicContext *ctx);
int quicSendData(struct QuicContext *ctx, const unsigned char *data, size_t len);
int quicReceiveData(struct QuicContext *ctx, unsigned char *data, size_t max_len);

#endif /* QUIC_MODULE_H */
