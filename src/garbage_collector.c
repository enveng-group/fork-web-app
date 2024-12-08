/**
 * \file garbage_collector.c
 * \brief Implements garbage collection functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/garbage_collector.h"
#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * \struct GCNode
 * \brief Node structure for garbage collector linked list.
 */
typedef struct GCNode
{
    void *ptr;           /**< Pointer to allocated memory */
    struct GCNode *next; /**< Pointer to next node */
} GCNode;

static GCNode *gcHead = NULL;

/**
 * \brief Initializes the garbage collector.
 */
void
initGarbageCollector (void)
{
    gcHead = NULL;
    printf ("Garbage collector initialized.\n");
}

/**
 * \brief Cleans up the garbage collector, freeing all allocated memory.
 */
void
cleanupGarbageCollector (void)
{
    GCNode *current = gcHead;
    while (current != NULL)
        {
            GCNode *next = current->next;
            free (current->ptr);
            free (current);
            current = next;
        }
    gcHead = NULL;
    printf ("Garbage collector cleaned up.\n");
}

/**
 * \brief Allocates memory and tracks it for garbage collection.
 *
 * \param size Size of the memory to allocate.
 * \return Pointer to the allocated memory, or NULL on failure.
 */
void *
gcMalloc (size_t size)
{
    GCNode *node;
    void *ptr = malloc (size);

    if (ptr == NULL)
        {
            logError ("Memory allocation failed for size %zu", size);
            return NULL;
        }

    node = (GCNode *)malloc (sizeof (GCNode));
    if (node == NULL)
        {
            free (ptr);
            logError ("Memory allocation failed for GCNode");
            return NULL;
        }

    /* Additional logic for tracking the allocated memory */

    return ptr;
}

/**
 * \brief Frees memory tracked by the garbage collector.
 *
 * \param ptr Pointer to memory to free.
 */
void
gcFree (void *ptr)
{
    GCNode **current = &gcHead;
    while (*current != NULL)
        {
            if ((*current)->ptr == ptr)
                {
                    GCNode *node = *current;
                    *current = node->next;
                    free (node->ptr);
                    free (node);
                    return;
                }
            current = &(*current)->next;
        }
}
