/**
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "config_loader.h"

size_t safeCopy(char *dst, const char *src, size_t size);
int validateConfig(config_t *config, const char *value);

#endif // VALIDATOR_H
