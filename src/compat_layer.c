/**
 * \file compat_layer.c
 * \brief Implements compatibility layer functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/compat_layer.h"
#include <openssl/evp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * \brief Initializes the SHA256 context.
 *
 * \param ctx SHA256 context.
 * \return 1 on success, 0 on failure.
 */
int compatSha256Init(EVP_MD_CTX *ctx)
{
    if (ctx == NULL)
    {
        return 0;
    }
    EVP_MD_CTX_init(ctx);
    return 1;
}

/**
 * \brief Updates the SHA-256 context with data.
 *
 * \param ctx The SHA-256 context.
 * \param data The data to update the context with.
 * \param len The length of the data.
 * \return 1 on success, 0 on failure.
 */
int compatSha256Update(EVP_MD_CTX *ctx, const void *data, size_t len)
{
    return EVP_DigestUpdate(ctx, data, len);
}

/**
 * \brief Finalizes the SHA256 context and produces the message digest.
 *
 * \param ctx SHA256 context.
 * \param md Buffer to store the message digest.
 * \return 1 on success, 0 on failure.
 */
int compatSha256Final(EVP_MD_CTX *ctx, unsigned char *md)
{
    if (ctx == NULL || md == NULL)
    {
        return 0;
    }
    return EVP_DigestFinal_ex(ctx, md, NULL);
}

/**
 * \brief Seeks to a position in a file stream.
 *
 * \param stream File stream.
 * \param offset Offset to seek to.
 * \param whence Position from where offset is applied.
 * \return 0 on success, -1 on failure.
 */
int compatFseek(FILE *stream, long offset, int whence)
{
    return fseek(stream, offset, whence);
}

/**
 * \brief Gets the current position in a file stream.
 *
 * \param stream File stream.
 * \return Current position in the file stream.
 */
long compatFtell(FILE *stream)
{
    return ftell(stream);
}
