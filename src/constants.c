/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "constants.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_loader.h"
#include "env_loader.h"
#include "error_handler.h"
#include "garbage_collector.h"
#include "logger.h"
#include "validator.h"

// Server constants
char SERVER_IP[SERVER_IP_SIZE];
int  SERVER_PORT;

// SSL constants
char SSL_CERT_FILE[SSL_CERT_FILE_SIZE];
char SSL_KEY_FILE[SSL_KEY_FILE_SIZE];

// Configuration file paths
const char *ENV_FILE    = ".env";
const char *CONFIG_FILE = "etc/config.ini";
const char *DOCUMENT_ROOT;
const char *REC_FILE_PATH;
const char *AUTH_FILE;

// Global variables
const int   GLOBAL_VAR = 0;
const char *APP_MODE;
double      VERSION;

// Initialize configuration with default values
const Config config = {.app_name      = APP_NAME,
                       .version       = CONFIG_VERSION,
                       .document_root = "",
                       .rec_file_path = "",
                       .auth_file     = ""};

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
            strncpy(SSL_CERT_FILE, trimmed_line + 14,
                    sizeof(SSL_CERT_FILE) - 1);
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
