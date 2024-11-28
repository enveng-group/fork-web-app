/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/config_loader.h"
#include "../include/validator.h"
#include "../include/logger.h"
#include "../include/error_handler.h"

int load_ini_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Error opening file
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");
        if (key && value) {
            if (validate_string(value) != 0) {
                handle_error("Invalid INI config value");
                fclose(file);
                return -1;
            }
            log_info("INI Config: %s = %s", key, value);
        }
    }

    fclose(file);
    return 0; // Success
}

int load_conf_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1; // Error opening file
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");
        if (key && value) {
            if (validate_string(value) != 0) {
                handle_error("Invalid CONF config value");
                fclose(file);
                return -1;
            }
            log_info("CONF Config: %s = %s", key, value);
        }
    }

    fclose(file);
    return 0; // Success
}
