/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef LOGGER_H
#define LOGGER_H

void init_logger(void);
void log_info(const char *message, ...);
void log_error(const char *message, ...);

#endif // LOGGER_H
