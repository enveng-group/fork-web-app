/**
 * \file config_loader.h
 * \brief Configuration loader header.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char *configPath;
extern int logLevel;
extern int maxConnections;
extern int timeout;

/**
 * \struct ConfigEntry
 * \brief Structure for a configuration entry.
 */
typedef struct {
    char *key;
    char *value;
} ConfigEntry;

/**
 * \struct Config
 * \brief Structure for configuration.
 */
typedef struct {
    char *mode;
    double version;
    ConfigEntry *entries;
    size_t entry_count;
} Config;

/* Function declarations */
int setConfigValue(Config *config, ConfigEntry entry);
void logFinalConfig(const Config *config);
int loadConfig(const char *filename, Config *config);

#endif /* CONFIG_LOADER_H */
