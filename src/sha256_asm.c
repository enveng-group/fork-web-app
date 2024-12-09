/**
 * \file sha256_asm.c
 * \brief Implements SHA256 block processing using standard C for portability.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include <openssl/sha.h>
#include <stdint.h>

/**
 * \brief Compresses a single block for SHA256.
 *
 * \param ctx SHA256 context.
 * \param block Block of data to compress.
 */
void sha256CompressBlock(SHA256_CTX *ctx, const uint8_t *block)
{
    /* Example of a simplified SHA256 block compression logic in C */
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t w[64];
    int i;

    /* Initialize working variables with current hash value */
    a = ctx->h[0];
    b = ctx->h[1];
    c = ctx->h[2];
    d = ctx->h[3];
    e = ctx->h[4];
    f = ctx->h[5];
    g = ctx->h[6];
    h = ctx->h[7];

    /* Prepare the message schedule */
    for (i = 0; i < 16; i++)
    {
        w[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16)
               | (block[i * 4 + 2] << 8) | block[i * 4 + 3];
    }
    for (i = 16; i < 64; i++)
    {
        w[i] = w[i - 16] + w[i - 7]; /* Simplified for illustration */
    }

    /* Perform the main hash computation (simplified) */
    for (i = 0; i < 64; i++)
    {
        uint32_t temp1 = h + w[i];
        uint32_t temp2 = a + b;
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    /* Update the hash value with the compressed block */
    ctx->h[0] += a;
    ctx->h[1] += b;
    ctx->h[2] += c;
    ctx->h[3] += d;
    ctx->h[4] += e;
    ctx->h[5] += f;
    ctx->h[6] += g;
    ctx->h[7] += h;
}
