/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include "config_loader.h"
#include "env_loader.h"
#include "error_handler.h"
#include "logger.h"
#include "garbage_collector.h"
#include "validator.h"
#include "constants.h"
#include "utils.h"

// Remove the definitions of initialize, cleanup, upgradeConfig, and main

size_t utils_safeCopy(char *dst, const char *src, size_t size)
{
    if (size == 0) return 0;
    size_t i;
    for (i = 0; i < size - 1 && src[i] != '\0'; i++)
    {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return i;
}
