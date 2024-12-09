/**
 * \file garbage_collector.h
 * \brief Header file for garbage collection functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <stddef.h> /* For size_t */
#include <pthread.h> /* For pthread_mutex_t */

/**
 * \struct GCNode
 * \brief Structure to hold garbage collection node information.
 */
typedef struct GCNode
{
    void *data;
    struct GCNode *next;
} GCNode;

/**
 * \struct GarbageCollector
 * \brief Structure to hold garbage collector information.
 */
typedef struct
{
    GCNode *head;
    pthread_mutex_t lock;
} GarbageCollector;

/* Function prototypes */
void initGarbageCollector(GarbageCollector *gc);
void addGarbage(GarbageCollector *gc, void *data);
void cleanupGarbageCollector(GarbageCollector *gc);
void *gcMalloc(GarbageCollector *gc, size_t size);
void gcFree(GarbageCollector *gc, void *ptr);
void *gcRealloc(void *ptr, size_t size);
char *gcStrdup(const char *str);

#endif /* GARBAGE_COLLECTOR_H */
