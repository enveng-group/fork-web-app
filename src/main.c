/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "../include/config_loader.h"
#include "../include/constants.h"
#include "../include/env_loader.h"
#include "../include/error_handler.h"
#include "../include/garbage_collector.h"
#include "../include/logger.h"
#include "../include/error_codes.h"
#include <stdio.h>
#include <stdlib.h>

#define ENV_FILE ".env"
#define CONFIG_FILE "etc/config.ini"

/* Function prototypes */
int initialize(void);
void cleanup(void);
void printLoadedConfig(void);
void printLoadedEnv(void);

int
main (void)
{
    int result;

    /* Initialize the application */
    result = initialize();
    if (result != SUCCESS)
    {
        handleError("Initialization failed");
        return EXIT_FAILURE;
    }

    /* Print loaded environment variables and configuration */
    printLoadedEnv();
    printLoadedConfig();

    /* Log application start */
    logInfo ("Application started: %s v%.1f", config.app_name, config.version);

    /* Application logic here */

    /* Cleanup the application */
    cleanup ();

    return EXIT_SUCCESS;
}

int
initialize (void)
{
    int result;

    /* Load environment variables */
    loadEnvironmentVariables();

    /* Load configuration */
    result = loadConfig();
    if (result != SUCCESS)
    {
        handleError("Failed to load configuration");
        return result;
    }

    /* Initialize garbage collector */
    initGarbageCollector ();

    return SUCCESS;
}

/**
 * Cleanup the application
 */
void
cleanup (void)
{
    /* Cleanup garbage collector */
    cleanupGarbageCollector ();
}

/**
 * Print loaded configuration
 */
void
printLoadedConfig (void)
{
    logInfo ("Loaded Configuration:");
    logInfo ("app_name: %s", config.app_name);
    logInfo ("version: %.1f", config.version);
    logInfo ("document_root: %s", config.document_root);
    logInfo ("rec_file_path: %s", config.rec_file_path);
    logInfo ("auth_file: %s", config.auth_file);
}

/**
 * Print loaded environment variables
 */
void
printLoadedEnv (void)
{
    logInfo ("Loaded Environment Variables:");
    logInfo ("SERVER_IP: %s", SERVER_IP);
    logInfo ("SERVER_PORT: %d", SERVER_PORT);
    logInfo ("SSL_CERT_FILE: %s", SSL_CERT_FILE);
    logInfo ("SSL_KEY_FILE: %s", SSL_KEY_FILE);
}
