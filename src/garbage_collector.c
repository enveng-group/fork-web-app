/**
 * \file garbage_collector.c
 * \brief Implements garbage collection functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/garbage_collector.h"
#include "../include/logger.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * \brief Initializes the garbage collector.
 *
 * \param gc The garbage collector to initialize.
 */
void initGarbageCollector(GarbageCollector *gc)
{
    if (gc == NULL)
    {
        return;
    }

    gc->head = NULL;
    pthread_mutex_init(&gc->lock, NULL);
}

/**
 * \brief Adds a data pointer to the garbage collector.
 *
 * \param gc The garbage collector.
 * \param data The data pointer to add.
 */
void addGarbage(GarbageCollector *gc, void *data)
{
    GCNode *node;

    if (gc == NULL || data == NULL)
    {
        return;
    }

    node = (GCNode *)malloc(sizeof(GCNode));
    if (node == NULL)
    {
        logError("Failed to allocate memory for GCNode");
        return;
    }

    node->data = data;
    node->next = NULL;

    pthread_mutex_lock(&gc->lock);

    node->next = gc->head;
    gc->head = node;

    pthread_mutex_unlock(&gc->lock);
}

/**
 * \brief Cleans up the garbage collector and frees all allocated memory.
 *
 * \param gc The garbage collector to clean up.
 */
void cleanupGarbageCollector(GarbageCollector *gc)
{
    GCNode *current;
    GCNode *next;

    if (gc == NULL)
    {
        return;
    }

    pthread_mutex_lock(&gc->lock);

    current = gc->head;
    while (current != NULL)
    {
        next = current->next;
        free(current->data);
        free(current);
        current = next;
    }

    gc->head = NULL;

    pthread_mutex_unlock(&gc->lock);
    pthread_mutex_destroy(&gc->lock);
}

/**
 * \brief Allocates memory and tracks it for garbage collection.
 *
 * \param gc The garbage collector.
 * \param size Size of the memory to allocate.
 * \return Pointer to the allocated memory, or NULL on failure.
 */
void *gcMalloc(GarbageCollector *gc, size_t size)
{
    GCNode *node;
    void *ptr;

    ptr = malloc(size);
    if (ptr == NULL)
    {
        logError("Memory allocation failed for size %zu", size);
        return NULL;
    }

    node = (GCNode *)malloc(sizeof(GCNode));
    if (node == NULL)
    {
        free(ptr);
        logError("Memory allocation failed for GCNode");
        return NULL;
    }

    node->data = ptr;
    node->next = NULL;

    pthread_mutex_lock(&gc->lock);
    node->next = gc->head;
    gc->head = node;
    pthread_mutex_unlock(&gc->lock);

    return ptr;
}

/**
 * \brief Frees memory tracked by the garbage collector.
 *
 * \param gc The garbage collector.
 * \param ptr Pointer to memory to free.
 */
void gcFree(GarbageCollector *gc, void *ptr)
{
    GCNode **current;

    current = &gc->head;
    while (*current != NULL)
    {
        if ((*current)->data == ptr)
        {
            GCNode *node = *current;
            *current = node->next;
            free(node->data);
            free(node);
            return;
        }
        current = &(*current)->next;
    }
}
