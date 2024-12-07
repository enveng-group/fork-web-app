/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <stdio.h>

typedef struct
{
    char   app_name[256];
    double version;  // Change from int to double
    char   document_root[256];
    char   rec_file_path[256];
    char   auth_file[256];
} config_t;

int loadIniConfig(const char *filename, config_t *config);

#endif  // CONFIG_LOADER_H
