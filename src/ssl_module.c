/**
 * \file ssl_module.c
 * \brief SSL module implementation.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/ssl_module.h"
#include "../include/compat_layer.h"
#include "../include/compat_types.h"
#include "../include/constants.h"
#include "../include/logger.h"
#include <errno.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_ERROR(cond, err_code)                                           \
    if (cond)                                                                 \
    {                                                                         \
        logError("Error: %s", strerror(err_code));                            \
        return NULL;                                                          \
    }

/* Function to log errors */
void logError (const char *format, ...);

/* Function to initialize SSL context */
SSL_CTX *
initializeSSLContext (void)
{
    SSL_CTX *ctx;

    /* Initialize OpenSSL library */
    SSL_library_init ();
    OpenSSL_add_all_algorithms ();
    SSL_load_error_strings ();

    ctx = SSL_CTX_new (SSLv23_method ());
    CHECK_ERROR (ctx == NULL, ENOMEM);

    return ctx;
}

/* Function to clean up SSL context */
void
cleanupSSLContext (SSL_CTX *ctx)
{
    if (ctx != NULL)
        {
            SSL_CTX_free (ctx);
        }
}

/* Additional functions using EVP API to avoid aggregate value issues */
int
someFunctionUsingEVP (void)
{
    EVP_MD_CTX *mdctx;

    mdctx = EVP_MD_CTX_new ();
    if (mdctx == NULL)
        {
            logError ("Failed to create EVP_MD_CTX: %s",
                      ERR_reason_error_string (ERR_get_error ()));
            return 0;
        }

    if (compatSha256Init (mdctx) != 1)
        {
            logError ("Failed to initialize SHA256 context");
            EVP_MD_CTX_free (mdctx);
            return 0;
        }

    /* Perform digest operations */
    if (compatSha256Update (mdctx, "data", 4) != 1)
        {
            logError ("Failed to update SHA256 context");
            EVP_MD_CTX_free (mdctx);
            return 0;
        }

    if (compatSha256Final (mdctx, NULL) != 1)
        {
            logError ("Failed to finalize SHA256 context");
            EVP_MD_CTX_free (mdctx);
            return 0;
        }

    EVP_MD_CTX_free (mdctx);
    return 1;
}

/* Function declaration with void parameter */
SSL_CTX *
createSslContext (void)
{
    return initializeSSLContext ();
}

/* Function to compute SHA256 hash */
unsigned char *
compatSha256 (const unsigned char *data, custom_uint64_t len,
              unsigned char *md)
{
    EVP_MD_CTX *mdctx;

    mdctx = EVP_MD_CTX_new ();
    if (mdctx == NULL)
        {
            logError ("Failed to create EVP_MD_CTX: %s",
                      ERR_reason_error_string (ERR_get_error ()));
            return NULL;
        }

    if (compatSha256Init (mdctx) != 1)
    {
        logError ("Failed to initialize SHA256 context");
        EVP_MD_CTX_free (mdctx);
        return NULL;
    }

    if (compatSha256Update (mdctx, data, (size_t)len.value) != 1)
    {
        logError ("Failed to update SHA256 context");
        EVP_MD_CTX_free (mdctx);
        return NULL;
    }

    if (compatSha256Final (mdctx, md) != 1)
    {
        logError ("Failed to finalize SHA256 context");
        EVP_MD_CTX_free (mdctx);
        return NULL;
    }

    EVP_MD_CTX_free (mdctx);
    return md;
}

/* Function to write data to SSL connection */
int
compatSslWrite (SSL *ssl, const void *buf, custom_uint64_t num)
{
    return SSL_write(ssl, buf, (int)num.value);
}

/* Function to read data from SSL connection */
int
compatSslRead (SSL *ssl, void *buf, custom_uint64_t num)
{
    return SSL_read(ssl, buf, (int)num.value);
}
