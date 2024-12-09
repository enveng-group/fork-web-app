/**
 * \file compat_layer.h
 * \brief Header file for compatibility layer functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef COMPAT_LAYER_H
#define COMPAT_LAYER_H

#include <openssl/evp.h>
#include <stddef.h>
#include <stdio.h>

int compatSha256Init(EVP_MD_CTX *ctx);
int compatSha256Update(EVP_MD_CTX *ctx, const void *data, size_t len);
int compatSha256Final(EVP_MD_CTX *ctx, unsigned char *md);

int compatFseek(FILE *stream, long offset, int whence);
long compatFtell(FILE *stream);

#endif /* COMPAT_LAYER_H */
