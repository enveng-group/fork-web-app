/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <stdio.h>
#include <stdarg.h>

/* Define constants */
#define MAX_BUFFER_SIZE          1024
#define DEFAULT_TIMEOUT          30
#define SERVER_IP_SIZE           16
#define SSL_CERT_FILE_SIZE       256
#define SSL_KEY_FILE_SIZE        256
#define APP_NAME                 "Web App"
#define CONFIG_VERSION           1.0
#define MAX_LINE_LENGTH          1024
#define MAX_ERROR_MESSAGE_LENGTH 256
#define CONFIG_FIELD_SIZE        256

/* Configuration structure */
typedef struct
{
    char app_name[CONFIG_FIELD_SIZE];
    double version;
    char document_root[CONFIG_FIELD_SIZE];
    char rec_file_path[CONFIG_FIELD_SIZE];
    char auth_file[CONFIG_FIELD_SIZE];
} Config;

typedef struct
{
    const char *key;  /* Add this member */
    const char *value;
} ConfigEntry;

char *trimWhitespace(char *str);
int isEmptyOrComment(const char *str);
int setConfigValue(Config *config, ConfigEntry entry);
int loadConfig(void);

void logFinalConfig(const Config *config);  /* Add this declaration */
void safeStrncpy(char *dest, const char *src, size_t n);  /* Add this declaration */

#endif /* CONFIG_LOADER_H */
