/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
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

extern Config config;  /* Declare config as extern */
extern const char *CONFIG_FILE;
extern const char *ENV_FILE;  /* Declare ENV_FILE as extern */

int loadConstants(const char *filename); /* Change to int */

#endif /* CONSTANTS_H */
