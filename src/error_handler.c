/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "error_handler.h"
#include "logger.h"

void handleError(const char *error_message)
{
    logError(error_message);
}
