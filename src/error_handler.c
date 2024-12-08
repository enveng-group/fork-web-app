/**
 * \file error_handler.c
 * \brief Implements error handling functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/error_handler.h"
#include "../include/logger.h"

/**
 * \brief Handles an error by logging it.
 *
 * \param error_message Error message to log.
 */
void
handleError (const char *error_message)
{
    handleLogError (error_message);
}

/**
 * \brief Logs an error message.
 *
 * \param error_message The error message to log.
 */
void handleLogError(const char *error_message)
{
    logError("Error: %s", error_message);
}
