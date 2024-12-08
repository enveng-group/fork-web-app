/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "../include/constants.h"
#include "../include/logger.h"
#include "../include/utils.h"
#include "../include/config_loader.h"
#include "../include/env_loader.h"
#include "../include/error_codes.h"  /* Include error_codes.h for SUCCESS */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Server constants */
char SERVER_IP[SERVER_IP_SIZE];
int SERVER_PORT;
char SSL_CERT_FILE[SSL_CERT_FILE_SIZE];
char SSL_KEY_FILE[SSL_KEY_FILE_SIZE];

/* Global configuration variable */
Config config = {
    "APP_MODE",
    CONFIG_VERSION,
    "",
    "",
    ""
};

/* Configuration file paths */
const char *ENV_FILE = ".env";  /* Define ENV_FILE */
const char *CONFIG_FILE = "etc/config.ini";
const char *DOCUMENT_ROOT;
const char *REC_FILE_PATH;
const char *AUTH_FILE;

/* Global variables */
const int GLOBAL_VAR = 0;
const char *APP_MODE;
double VERSION;

int loadConstants(const char *filename)
{
    /* Implementation of loadConstants function */
    (void)filename;  /* Suppress unused parameter warning */

    /* If there is no implementation, return SUCCESS */
    return SUCCESS;
}
