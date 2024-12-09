/**
 * \file main.c
 * \brief Main entry point for the application.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/atomic_ops.h"
#include "../include/compat.h"
#include "../include/config_loader.h"
#include "../include/constants.h"
#include "../include/csv_utils.h"
#include "../include/data_structures.h"
#include "../include/env_loader.h"
#include "../include/error_codes.h"
#include "../include/error_handler.h"
#include "../include/garbage_collector.h"
#include "../include/http_parser.h"
#include "../include/http_response.h"
#include "../include/logger.h"
#include "../include/project.h"
#include "../include/rec_utils.h"
#include "../include/records.h"
#include "../include/socket_module.h"
#include "../include/ssl_module.h"
#include "../include/static_file_handler.h"
#include "../include/test_framework.h"
#include "../include/validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define ENV_FILE ".env"
#define CONFIG_FILE "etc/config.ini"
#define LOG_FILE "app.log"

/* Global configuration variable */
Config config;

/* Global garbage collector */
GarbageCollector gc;

/* Function prototypes */
int initialize(void);
void cleanup(void);
void printLoadedConfig(void);
void startServer(void);

/**
 * \brief Main function.
 *
 * \return Exit status.
 */
int main(void)
{
    /* Initialize the config structure */
    memset(&config, 0, sizeof(Config));

    /* Initialize the garbage collector */
    initGarbageCollector(&gc);

    if (loadConfig(CONFIG_FILE, &config) != SUCCESS)
    {
        logError("Failed to load configuration");
        return EXIT_FAILURE;
    }

    logFinalConfig(&config);

    if (initialize() != SUCCESS)
    {
        logError("Initialization failed");
        return EXIT_FAILURE;
    }

    printLoadedConfig();

    /* Start the server */
    startServer();

    cleanup();

    /* Free the configuration resources */
    freeConfig(&config);

    return EXIT_SUCCESS;
}

/**
 * \brief Initializes the application.
 *
 * \return SUCCESS on success, otherwise an error code.
 */
int initialize(void)
{
    initLogger(LOG_FILE);
    logInfo("Application starting...");

    if (loadEnvConfig(ENV_FILE) != SUCCESS)
    {
        logError("Failed to load environment configuration");
        return ERROR_CONFIG_LOAD;
    }

    if (loadConfig(CONFIG_FILE, &config) != SUCCESS)
    {
        logError("Failed to load configuration");
        return ERROR_CONFIG_LOAD;
    }

    logFinalConfig(&config);
    logInfo("All modules initialized successfully");
    return SUCCESS;
}

/**
 * \brief Cleans up resources before exiting the application.
 */
void cleanup(void)
{
    cleanupGarbageCollector(&gc);
    closeLogger();
}

/**
 * \brief Prints the loaded configuration.
 */
void printLoadedConfig(void)
{
    logInfo("Loaded configuration:");
    logInfo("Server IP: %s", config.server_ip);
    logInfo("Log Level: %d", config.log_level);
    logInfo("Max Connections: %d", config.max_connections);
}

/**
 * \brief Starts the server and handles incoming connections.
 */
void startServer(void)
{
    int server_fd, client_fd;
    pthread_t thread_id;
    int *client_fd_ptr;

    server_fd = createAndBindSocket();
    if (server_fd < 0)
    {
        logError("Failed to create and bind socket");
        return;
    }

    logInfo("Server is listening on %s:%d", getServerIp(), getServerPort());

    while (1)
    {
        client_fd = acceptConnection(server_fd);
        if (client_fd < 0)
        {
            logError("Failed to accept connection");
            continue;
        }

        client_fd_ptr = malloc(sizeof(int));
        if (client_fd_ptr == NULL)
        {
            logError("Failed to allocate memory for client_fd_ptr");
            close(client_fd);
            continue;
        }

        *client_fd_ptr = client_fd;
        if (pthread_create(&thread_id, NULL, handle_client, client_fd_ptr) != 0)
        {
            logError("Failed to create thread for client");
            free(client_fd_ptr);
            close(client_fd);
        }
        else
        {
            pthread_detach(thread_id);
        }
    }

    close(server_fd);
}
