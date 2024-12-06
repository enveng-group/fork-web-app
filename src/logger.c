/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "logger.h"

// logger.c
void initLogger(void)
{
    // Initialize logging system
}

void logInfo(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    vdprintf(STDOUT_FILENO, message, args);
    dprintf(STDOUT_FILENO, "\n");
    va_end(args);
}

void logError(const char *message, ...)
{
    va_list args;
    va_start(args, message);
    vdprintf(STDERR_FILENO, message, args);
    dprintf(STDERR_FILENO, "\n");
    va_end(args);
}
