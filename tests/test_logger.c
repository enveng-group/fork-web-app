/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include "logger.h"

int main(void) {
    log_info("This is a test.");
    log_error("This is an error test.");
    return 0;
}
