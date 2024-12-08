/**
 * \file logger.h
 * \brief Header file for logging functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef LOGGER_H
#define LOGGER_H

void initLogger(const char *logFile);
void closeLogger(void);
void loggerLog(const char *message);
void logInfo(const char *format, ...);
void logError(const char *format, ...);
const char *loggerGetLastMessage(void);
int loggerGetLastError(void);

#endif /* LOGGER_H */
