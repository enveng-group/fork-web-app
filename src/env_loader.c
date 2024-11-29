/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

#include "../lib/stdio.h"
#include <stdlib.h>
#include <string.h>
#include "../include/env_loader.h"
#include "../include/validator.h"
#include "../include/logger.h"
#include "../include/error_handler.h"

int load_env_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (key && value) {
            config_t config;
            config.key = strdup(key);
            config.value = strdup(value);

            // Process the config as needed
            printf("Key: %s, Value: %s\n", config.key, config.value);

            // Free allocated memory
            free(config.key);
            free(config.value);
        }
    }

    fclose(file);
    return 0;
}
