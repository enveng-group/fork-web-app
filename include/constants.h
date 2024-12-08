/**
 * \file constants.h
 * \brief Defines constants and global variables.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "config_loader.h"

#define SERVER_IP_SIZE 16
#define SSL_CERT_FILE_SIZE 256
#define SSL_KEY_FILE_SIZE 256

extern char SERVER_IP[SERVER_IP_SIZE];
extern int SERVER_PORT;
extern char SSL_CERT_FILE[SSL_CERT_FILE_SIZE];
extern char SSL_KEY_FILE[SSL_KEY_FILE_SIZE];

extern Config config; /**< Global configuration variable */
extern const char *CONFIG_FILE;
extern const char *ENV_FILE; /**< Environment file path */

/**
 * \brief Loads constants from a file.
 *
 * \param filename Name of the file to load.
 * \return SUCCESS on success, otherwise an error code.
 */
int loadConstants (const char *filename);

#endif /* CONSTANTS_H */
