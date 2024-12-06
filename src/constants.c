/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "constants.h"
#include "env_loader.h"
#include "config_loader.h"
#include "logger.h"
#include "error_handler.h"
#include "garbage_collector.h"
#include "validator.h"

// Server constants
char SERVER_IP[256];
int SERVER_PORT;

// SSL constants
char SSL_CERT_FILE[256];
char SSL_KEY_FILE[256];

// Configuration file paths
const char *ENV_FILE = ".env";
const char *CONFIG_FILE = "etc/config.ini";
const char *DOCUMENT_ROOT;
const char *REC_FILE_PATH;
const char *AUTH_FILE;

// Application constants
const int global_var = 0;
const char *app_mode;
double version;

// Initialize configuration with default values
const Config config = {
    .app_name = "web_app",
    .version = 1.0,
    .document_root = "",
    .rec_file_path = "",
    .auth_file = ""
};

void loadConstants(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        logError("Failed to open constants file: %s", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        // Trim leading and trailing whitespace
        char *trimmed_line = line;
        while (isspace((unsigned char)*trimmed_line)) trimmed_line++;
        size_t trimmed_len = strlen(trimmed_line);
        if (trimmed_len == 0)
        {
            continue;
        }
        char *end = trimmed_line + trimmed_len - 1;
        while (end > trimmed_line && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        // Parse the line and update the corresponding variable
        if (strncmp(trimmed_line, "SERVER_IP=", 10) == 0)
        {
            strncpy(SERVER_IP, trimmed_line + 10, sizeof(SERVER_IP) - 1);
            SERVER_IP[sizeof(SERVER_IP) - 1] = '\0';  // Ensure null termination
        }
        else if (strncmp(trimmed_line, "SSL_CERT_FILE=", 14) == 0)
        {
            strncpy(SSL_CERT_FILE, trimmed_line + 14, sizeof(SSL_CERT_FILE) - 1);
            SSL_CERT_FILE[sizeof(SSL_CERT_FILE) - 1] = '\0';
        }
        else if (strncmp(trimmed_line, "SSL_KEY_FILE=", 13) == 0)
        {
            strncpy(SSL_KEY_FILE, trimmed_line + 13, sizeof(SSL_KEY_FILE) - 1);
            SSL_KEY_FILE[sizeof(SSL_KEY_FILE) - 1] = '\0';
        }
    }

    fclose(file);
}
