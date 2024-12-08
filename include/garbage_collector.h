/**
 * \file garbage_collector.h
 * \brief Header file for garbage collection functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <stdlib.h>

/**
 * \brief Initializes the garbage collector.
 */
void initGarbageCollector (void);

/**
 * \brief Cleans up the garbage collector, freeing all allocated memory.
 */
void cleanupGarbageCollector (void);

/**
 * \brief Allocates memory and tracks it in the garbage collector.
 *
 * \param size Size of memory to allocate.
 * \return Pointer to allocated memory, or NULL on failure.
 */
void *gcMalloc (size_t size);

/**
 * \brief Frees memory tracked by the garbage collector.
 *
 * \param ptr Pointer to memory to free.
 */
void gcFree (void *ptr);

#endif /* GARBAGE_COLLECTOR_H */
