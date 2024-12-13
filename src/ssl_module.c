/* src/ssl_module.c */
#include <stdio.h>
#include "../include/ssl_module.h"
#include "../include/logger.h"

SSL_CTX* createSslContext(const char* cert_file, const char* key_file)
{
    SSL_CTX* ctx;

    /* Initialize OpenSSL */
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    /* Create new context using TLS method */
    ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        logError("Failed to create SSL context");
        return NULL;
    }

    /* Set minimum TLS version to 1.2 */
    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION)) {
        logError("Failed to set minimum TLS version");
        SSL_CTX_free(ctx);
        return NULL;
    }

    /* Load certificate and private key */
    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        logError("Failed to load certificate file");
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
        logError("Failed to load private key file");
        SSL_CTX_free(ctx);
        return NULL;
    }

    /* Verify private key */
    if (!SSL_CTX_check_private_key(ctx)) {
        logError("Private key does not match certificate");
        SSL_CTX_free(ctx);
        return NULL;
    }

    return ctx;
}

void cleanupSslContext(SSL_CTX* ctx)
{
    if (ctx) {
        SSL_CTX_free(ctx);
    }
    EVP_cleanup();
    ERR_free_strings();
}
