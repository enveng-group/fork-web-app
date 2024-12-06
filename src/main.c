/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include "config_loader.h"
#include "env_loader.h"
#include "error_handler.h"
#include "logger.h"
#include "garbage_collector.h"
#include "validator.h"
#include "constants.h"

// Function prototypes
void initialize(void);
void hello(void);
void cleanup(void);
void upgradeConfig(double old_version);

int main(void)
{
    // Initialize the application
    initialize();

    // Application logic
    printf("Hello, world!\n");

    // Use SERVER_IP and SERVER_PORT
    printf("Server IP: %s\n", SERVER_IP);
    printf("Server Port: %d\n", SERVER_PORT);

    // Cleanup the application
    cleanup();

    return 0;
}

void hello(void)
{
    printf("Hello, world!\n");
}

/**
 * Initialize the application
 */
void initialize(void)
{
    // Initialize garbage collector
    initGarbageCollector();

    // Initialize logger
    initLogger();

    // Load constants from .env and config.ini files
    loadEnvConfig(ENV_FILE);
    loadConstants(CONFIG_FILE);

    // Upgrade configuration if needed
    if (config.version < CURRENT_CONFIG_VERSION)
    {
        upgradeConfig(config.version);
    }

    // Log application start
    logInfo("Application started: %s v%.1f", config.app_name, config.version);
}

/**
 * Cleanup the application
 */
void cleanup(void)
{
    // Cleanup garbage collector
    cleanupGarbageCollector();
}

/**
 * Upgrade configuration to the current version.
 * This function applies necessary transformations to upgrade the configuration to the current version.
 * @param old_version The old version of the configuration.
 */
void upgradeConfig(double old_version)
{
    if (old_version < 2.0)
    {
        // Add upgrade logic here
    }
    // Add more transformations for future versions as needed
}
