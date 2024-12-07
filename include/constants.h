/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Server constants
#define SERVER_IP_SIZE 256
extern char SERVER_IP[SERVER_IP_SIZE];
extern int  SERVER_PORT;

// SSL constants
#define SSL_CERT_FILE_SIZE 256
#define SSL_KEY_FILE_SIZE 256
extern char SSL_CERT_FILE[SSL_CERT_FILE_SIZE];
extern char SSL_KEY_FILE[SSL_KEY_FILE_SIZE];

// Configuration file paths
extern const char *ENV_FILE;
extern const char *CONFIG_FILE;
extern const char *DOCUMENT_ROOT;
extern const char *REC_FILE_PATH;
extern const char *AUTH_FILE;

// Application constants
#define APP_NAME "web_app"
#define CURRENT_CONFIG_VERSION 1.0
#define CONFIG_VERSION 1.0

// Global variables
extern const int   GLOBAL_VAR;
extern const char *APP_MODE;
extern double      VERSION;

/**
 * Configuration structure.
 */
typedef struct
{
    char app_name[256];      /**< Application name */
    char version[32];        /**< Version string */
    char document_root[256]; /**< Document root path */
    char rec_file_path[256]; /**< Record file path */
    char auth_file[256];     /**< Authentication file path */
} Config;

extern const Config config;

// Function to load constants from .env and config.ini files
void loadConstants(const char *filename);

#endif  // CONSTANTS_H
