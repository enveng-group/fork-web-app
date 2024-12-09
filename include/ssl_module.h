/**
 * \file ssl_module.h
 * \brief Header file for SSL context creation using OpenSSL API.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef SSL_MODULE_H
#define SSL_MODULE_H

#include <openssl/evp.h>

/**
 * \brief Creates an SSL context.
 *
 * \return Pointer to the created SSL context, or NULL on failure.
 */
SSL_CTX *createSslContext (void);

/**
 * \brief Wrapper for SHA256_Final using custom_uint64_t.
 *
 * \param ctx SHA256 context.
 * \param md Buffer to store the message digest.
 * \return 1 on success, 0 on failure.
 */
int compatSha256Final (EVP_MD_CTX *ctx, unsigned char *md);

/**
 * \brief Wrapper for EVP_DigestInit_ex using custom_uint64_t.
 *
 * \param ctx SHA256 context.
 * \return 1 on success, 0 on failure.
 */
int compatSha256Init (EVP_MD_CTX *ctx);

#endif /* SSL_MODULE_H */
