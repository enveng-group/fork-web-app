/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <stdlib.h>

void initGarbageCollector(void);
void cleanupGarbageCollector(void);
void* gcMalloc(size_t size);
void gcFree(void *ptr);

#endif // GARBAGE_COLLECTOR_H
