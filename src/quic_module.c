/* src/quic_module.c */
/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "../include/quic_module.h"
#include "../include/logger.h"
#include "../include/ssl_module.h"
#include "../include/utils.h"

struct QuicContext* initializeQuicContext(int socket_fd, void *ssl_ctx,
                                        int max_streams, int timeout_ms)
{
    struct QuicContext *ctx;
    int flags;

    if (socket_fd < 0 || !ssl_ctx) {
        logError("Invalid parameters for QUIC context initialization");
        return NULL;
    }

    ctx = (struct QuicContext *)malloc(sizeof(struct QuicContext));
    if (!ctx) {
        logError("Failed to allocate QUIC context");
        return NULL;
    }

    /* Initialize context */
    memset(ctx, 0, sizeof(struct QuicContext));
    ctx->socket_fd = socket_fd;
    ctx->ssl_context = ssl_ctx;
    ctx->max_streams = max_streams > 0 ? max_streams : QUIC_MAX_STREAMS;
    ctx->timeout_ms = timeout_ms > 0 ? timeout_ms : QUIC_DEFAULT_TIMEOUT_MS;
    ctx->buffer_size = QUIC_MAX_DATA_LENGTH;

    /* Allocate buffers */
    ctx->send_buffer = (unsigned char *)malloc(ctx->buffer_size);
    if (!ctx->send_buffer) {
        logError("Failed to allocate send buffer");
        free(ctx);
        return NULL;
    }

    ctx->recv_buffer = (unsigned char *)malloc(ctx->buffer_size);
    if (!ctx->recv_buffer) {
        logError("Failed to allocate receive buffer");
        free(ctx->send_buffer);
        free(ctx);
        return NULL;
    }

    /* Set socket to non-blocking mode */
    flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
        logError("Failed to get socket flags");
        goto cleanup;
    }

    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        logError("Failed to set socket non-blocking");
        goto cleanup;
    }

    ctx->is_initialized = 1;
    return ctx;

cleanup:
    if (ctx) {
        free(ctx->send_buffer);
        free(ctx->recv_buffer);
        free(ctx);
    }
    return NULL;
}

void cleanupQuicContext(struct QuicContext *ctx)
{
    if (!ctx) {
        return;
    }

    if (ctx->send_buffer) {
        secureZeroMemory(ctx->send_buffer, ctx->buffer_size);
        free(ctx->send_buffer);
    }

    if (ctx->recv_buffer) {
        secureZeroMemory(ctx->recv_buffer, ctx->buffer_size);
        free(ctx->recv_buffer);
    }

    secureZeroMemory(ctx, sizeof(struct QuicContext));
    free(ctx);
}

int quicSendData(struct QuicContext *ctx, const unsigned char *data, size_t len)
{
    ssize_t sent;

    if (!ctx || !ctx->is_initialized || !data || len == 0) {
        return -1;
    }

    if (len > ctx->buffer_size) {
        logError("Data length exceeds maximum QUIC packet size");
        return -1;
    }

    memcpy(ctx->send_buffer, data, len);
    sent = send(ctx->socket_fd, ctx->send_buffer, len, 0);

    if (sent < 0) {
        logError("Failed to send QUIC data: %s", strerror(errno));
        return -1;
    }

    return (int)sent;
}

int quicReceiveData(struct QuicContext *ctx, unsigned char *data, size_t max_len)
{
    ssize_t received;

    if (!ctx || !ctx->is_initialized || !data || max_len == 0) {
        return -1;
    }

    received = recv(ctx->socket_fd, ctx->recv_buffer,
                   max_len < ctx->buffer_size ? max_len : ctx->buffer_size, 0);

    if (received < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            logError("Failed to receive QUIC data: %s", strerror(errno));
            return -1;
        }
        return 0;
    }

    if (received > 0) {
        memcpy(data, ctx->recv_buffer, received);
    }

    return (int)received;
}
