/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

typedef struct {
    char *key;
    char *value;
    char *app_name;
    double version;
    char *log_level;
    int enable_feature_x;
    int enable_feature_y;
    int enable_feature_z;
    int max_retries;
    char *app_mode;
} config_t;

int load_ini_config(const char *filename);
int load_conf_config(const char *filename);

#endif // CONFIG_LOADER_H
