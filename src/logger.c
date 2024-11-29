/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

#include "../lib/stdio.h"
#include <stdarg.h>
#include "../include/logger.h"

// logger.c
void init_logger(void) {
    // Initialize logging system
}

void log_info(const char *message, ...) {
    va_list args;
    va_start(args, message);
    vprintf("%s", args);
    printf("\n");
    va_end(args);
}

void log_error(const char *message, ...) {
    va_list args;
    va_start(args, message);
    vfprintf(stderr, "%s", args);
    fprintf(stderr, "\n");
    va_end(args);
}
