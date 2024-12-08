/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "../include/garbage_collector.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct GCNode
{
    void *ptr;
    struct GCNode *next;
} GCNode;

static GCNode *gcHead = NULL;

void
initGarbageCollector (void)
{
    gcHead = NULL;
    printf("Garbage collector initialized.\n");
}

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
    printf("Garbage collector cleaned up.\n");
}

void *
gcMalloc (size_t size)
{
    GCNode *node;
    void *ptr = malloc(size);

    if (ptr == NULL)
    {
        return NULL;
    }

    node = (GCNode *)malloc(sizeof(GCNode));
    if (node == NULL)
    {
        free(ptr);
        return NULL;
    }

    node->ptr = ptr;
    node->next = gcHead;
    gcHead = node;

    return ptr;
}

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

/* Initialize the garbage collector */
void initializeGarbageCollector(void)
{
    initGarbageCollector();
}

/* Collect garbage */
void collectGarbage(void)
{
    cleanupGarbageCollector();
}
