/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include "../include/config_loader.h"
#include "../include/env_loader.h"
#include "../include/logger.h"
#include "../include/garbage_collector.h"
#include "../include/error_handler.h"
#include "../include/validator.h"
#include "../include/hello.h"

// Define constants and global variables
#define APP_NAME "App"
#define CURRENT_CONFIG_VERSION 2.0
#define CONFIG_VERSION 2.0
const int global_var = 0;

// Initialize configuration with default values
typedef struct {
    const char *app_name;
    const char *log_level;
    const char *app_mode;
    double version;
} Config;

const Config config = {
    .app_name = "DefaultApp",
    .log_level = "INFO",
    .app_mode = "production",
    .version = 1.0
};

// Function prototypes
void initialize(void);
void cleanup(void);
void upgrade_config(double old_version);

int main(int argc, char *argv[])
{
    // Initialization
    initialize();
    // Example usage of validator functions
    config_t config;
    if (validateConfig(&config, "some_value") != 0)
    {
        log_error("Invalid configuration");
        return 1;
    }

    if (validate_integer("123") != 0)
    {
        logError("Invalid integer");
        return 1;
    }

    if (validate_double("123.45") != 0)
    {
        logError("Invalid double");
        return 1;
    }

    if (validate_string("example") != 0)
    {
        logError("Invalid string");
        return 1;
    }

    if (validate_boolean("true") != 0)
    {
        logError("Invalid boolean");
        return 1;
    }

    // Main loop
    print_hello();
    // Cleanup
    cleanup();
    return 0;
}

/**
 * Initialize the application
 */
void initialize(void)
{
    // Initialize logger
    init_logger();
    // Load configuration
    if (load_ini_config("../etc/config.ini") != 0)
    {
        handle_error("Failed to load INI configuration");
    }
    if (load_conf_config("../etc/config.conf") != 0)
    {
        handle_error("Failed to load CONF configuration");
    }
    if (load_env_config("../.env") != 0)
    {
        handle_error("Failed to load ENV configuration");
    }
    // Upgrade configuration if needed
    if (config.version < CURRENT_CONFIG_VERSION)
    {
        upgrade_config(config.version);
    }
    // Log application start
    log_info("Application started: %s v%.1f", config.app_name, config.version);
}

/**
 * Cleanup the application
 */
void cleanup(void)
{
    // Cleanup resources
    cleanup_garbage_collector();
}

/**
 * Upgrade configuration to the current version.
 * This function applies necessary transformations to upgrade the configuration to the current version.
 * @param old_version The old version of the configuration.
 */
void upgrade_config(double old_version)
{
    if (old_version < 2.0)
    {
        // Add upgrade logic here
    }
    // Add more transformations for future versions as needed
}
