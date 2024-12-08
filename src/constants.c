/**
 * \file constants.c
 * \brief Defines constants and global variables.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/constants.h"
#include "../include/config_loader.h"
#include "../include/env_loader.h"
#include "../include/error_codes.h" /* Include error_codes.h for SUCCESS */
#include "../include/logger.h"
#include "../include/utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define CONFIG_VERSION if not defined */
#ifndef CONFIG_VERSION
#define CONFIG_VERSION 1.0
#endif

/* Server constants */
char SERVER_IP[SERVER_IP_SIZE];
int SERVER_PORT;
char SSL_CERT_FILE[SSL_CERT_FILE_SIZE];
char SSL_KEY_FILE[SSL_KEY_FILE_SIZE];

/* Global configuration variable */
Config config = { "APP_MODE", CONFIG_VERSION, NULL, 0 };

/* Configuration file paths */
const char *ENV_FILE = ".env"; /* Define ENV_FILE */
const char *CONFIG_FILE = "etc/config.ini";
const char *DOCUMENT_ROOT;
const char *REC_FILE_PATH;
const char *AUTH_FILE;

/* Global variables */
const int GLOBAL_VAR = 0;
const char *APP_MODE;
double VERSION;

/**
 * \brief Loads constants from a file.
 *
 * \param filename Name of the file to load.
 * \return SUCCESS on success, otherwise an error code.
 */
int
loadConstants (const char *filename)
{
    /* Implementation of loadConstants function */
    (void)filename; /* Suppress unused parameter warning */

    /* If there is no implementation, return SUCCESS */
    return SUCCESS;
}
