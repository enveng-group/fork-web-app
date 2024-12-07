/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdio.h>

void initLogger(void);
void logInfo(const char *format, ...);
void logError(const char *message, ...);

#endif  // LOGGER_H
