/* include/ssl_module.h */
#ifndef SSL_MODULE_H
#define SSL_MODULE_H

#include <openssl/ssl.h>
#include <openssl/err.h>

/**
 * Creates and configures an SSL context
 * @param cert_file Path to SSL certificate file
 * @param key_file Path to SSL private key file
 * @return Configured SSL_CTX* or NULL on error
 */
SSL_CTX* createSslContext(const char* cert_file, const char* key_file);

/**
 * Cleans up SSL context and resources
 * @param ctx SSL context to clean up
 */
void cleanupSslContext(SSL_CTX* ctx);

#endif /* SSL_MODULE_H */
