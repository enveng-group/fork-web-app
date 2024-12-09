/**
 * \file env_loader.c
 * \brief Implements environment variable loading functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/env_loader.h"
#include "../include/constants.h"
#include "../include/utils.h"
#include "../include/error_codes.h"
#include "../include/logger.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define ENV_FILE ".env"
#define SERVER_IP_SIZE 16
#define SSL_CERT_FILE_SIZE 256
#define SSL_KEY_FILE_SIZE 256

extern char server_ip[SERVER_IP_SIZE];
extern int server_port;
extern char SSL_CERT_FILE[SSL_CERT_FILE_SIZE];
extern char SSL_KEY_FILE[SSL_KEY_FILE_SIZE];

/**
 * \enum LINE_SIZE
 * \brief Defines the maximum size of a line in the environment file.
 */
enum
{
    LINE_SIZE = 1024
};

/**
 * \brief Loads environment configuration from a file.
 *
 * \param filename The name of the file to load.
 * \return SUCCESS on success, otherwise an error code.
 */
int loadEnvConfig(const char *filename)
{
    FILE *file;
    char line[MAX_LINE_LENGTH];

    file = fopen(filename, "r");
    if (!file)
    {
        perror("Failed to open environment config file");
        return ERROR_FILE_OPEN;
    }

    while (fgets(line, sizeof(line), file))
    {
        logInfo("Loaded environment variable: %s", line);
    }

    fclose(file);
    return SUCCESS;
}
