/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Server constants
extern char SERVER_IP[256];
extern int SERVER_PORT;

// SSL constants
extern char SSL_CERT_FILE[256];
extern char SSL_KEY_FILE[256];

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
extern const int global_var;
extern const char *app_mode;
extern double version;

// Configuration structure
typedef struct {
    const char *app_name;
    const char *log_level;
    const char *app_mode;
    double version; // Ensure this matches the type in config_loader.h
    char document_root[256];
    char rec_file_path[256];
    char auth_file[256];
} Config;

extern const Config config;

// Function to load constants from .env and config.ini files
void loadConstants(const char *filename);

#endif // CONSTANTS_H
