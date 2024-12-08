/**
 * \file main.c
 * \brief Main entry point for the application.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/logger.h"
#include "../include/config_loader.h"
#include "../include/constants.h"
#include "../include/env_loader.h"
#include "../include/error_codes.h"
#include "../include/error_handler.h"
#include "../include/garbage_collector.h"
#include "../include/records.h"
#include "../include/rec_utils.h"
#include "../include/csv_utils.h"
#include "../include/socket_module.h"
#include <stdio.h>
#include <stdlib.h>

#define ENV_FILE ".env"
#define CONFIG_FILE "etc/config.ini"
#define LOG_FILE "app.log"

/* Function prototypes */
int initialize(void);
void cleanup(void);
void printLoadedConfig(void);

/**
 * \brief Main function.
 *
 * \return EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int
main (void)
{
    int sockfd;

    printf("Starting application...\n");

    if (initialize() != SUCCESS)
    {
        printf("Initialization failed.\n");
        return 1;
    }

    printf("Initialization successful.\n");

    /* Create and bind the socket */
    sockfd = createSocket();
    if (sockfd < 0)
    {
        printf("Failed to create and bind socket.\n");
        cleanup();
        return 1;
    }

    printf("Socket created and bound successfully.\n");

    /* Your main application logic here */

    cleanup();
    printf("Application finished.\n");

    return 0;
}

/**
 * \brief Initializes the application.
 *
 * \return SUCCESS on success, otherwise an error code.
 */
int
initialize (void)
{
    initLogger(LOG_FILE);
    printf("Logger initialized.\n");

    if (loadEnvConfig(ENV_FILE) != SUCCESS)
    {
        printf("Failed to load environment configuration.\n");
        return ERROR_CONFIG_LOAD;
    }
    printf("Environment configuration loaded.\n");

    if (loadConfig(CONFIG_FILE, &config) != SUCCESS)
    {
        printf("Failed to load configuration.\n");
        return ERROR_CONFIG_LOAD;
    }
    printf("Configuration loaded.\n");

    printLoadedConfig();

    initGarbageCollector();
    printf("Garbage collector initialized.\n");

    return SUCCESS;
}

/**
 * \brief Cleans up the application.
 */
void
cleanup (void)
{
    cleanupGarbageCollector();
    closeLogger();
}

/**
 * \brief Prints the loaded configuration.
 */
void
printLoadedConfig (void)
{
    loggerLog("Loaded configuration:");
    loggerLog("App Name: web_app");
    loggerLog("Version: 1.0");
    loggerLog("Document Root: /var/www/html");
    loggerLog("Record File Path: /var/www/data/records.rec");
    loggerLog("Auth File: /etc/server/auth.passwd");
}
