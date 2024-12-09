/**
 * \file config_loader.h
 * \brief Header file for configuration loading functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include "constants.h"
#include "error_codes.h"
#include <stddef.h> /* For size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONFIG_ENTRIES 100
#define MAX_LINE_LENGTH    256
#define MAX_KEY_LENGTH     256
#define MAX_VALUE_LENGTH   256
#define SUCCESS            0

typedef struct
{
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} ConfigEntry;

typedef struct
{
    char server_ip[16];
    int server_port;
    char *app_mode;
    double config_version;
    int max_connections;
    char ssl_cert_file[256];
    char ssl_key_file[256];
    char app_name[256];
    char document_root[256];
    char rec_file_path[256];
    char auth_file[256];
    int log_level;
    ConfigEntry entries[MAX_CONFIG_ENTRIES];
    int entry_count;
} Config;

int loadConfig(const char *filename, Config *config);
void freeConfig(Config *config);
void logFinalConfig(const Config *config);

#endif /* CONFIG_LOADER_H */
