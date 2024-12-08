/**
 * \file error_handler.h
 * \brief Header file for error handling functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/**
 * \brief Handles an error by logging it.
 *
 * \param error_message Error message to log.
 */
void handleError (const char *error_message);

/**
 * \brief Logs an error message.
 *
 * \param error_message Error message to log.
 */
void handleLogError (const char *error_message);

#endif /* ERROR_HANDLER_H */
