/**
 * \file logger.c
 * \brief Implementation of the logger module.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static FILE *logFile = NULL;
static char lastMessage[256];
static int lastError = 0;

void initLogger(const char *logFileName)
{
    logFile = fopen(logFileName, "a");
    if (logFile == NULL)
    {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
}

void closeLogger(void)
{
    if (logFile != NULL)
    {
        fclose(logFile);
        logFile = NULL;
    }
}

void loggerLog(const char *message)
{
    if (logFile != NULL)
    {
        fprintf(logFile, "%s\n", message);
        fflush(logFile);
        strncpy(lastMessage, message, sizeof(lastMessage) - 1);
        lastMessage[sizeof(lastMessage) - 1] = '\0';
    }
}

void logInfo(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (logFile != NULL)
    {
        vfprintf(logFile, format, args);
        fprintf(logFile, "\n");
        fflush(logFile);
    }
    va_end(args);
}

void logError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (logFile != NULL)
    {
        vfprintf(logFile, format, args);
        fprintf(logFile, "\n");
        fflush(logFile);
    }
    va_end(args);
}

const char *loggerGetLastMessage(void)
{
    return lastMessage;
}

int loggerGetLastError(void)
{
    return lastError;
}
