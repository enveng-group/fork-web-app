/**
 * \file data_structures.c
 * \brief Implements data structure handling functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/data_structures.h"
#include "../include/garbage_collector.h"
#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function declarations */
void *gcMalloc (size_t size);
void gcFree (void *ptr);

/**
 * \brief Initializes the dictionary.
 *
 * \param dict Pointer to the dictionary to initialize.
 */
void
initDictionary (Dictionary *dict)
{
    dict->keys = NULL;
    dict->values = NULL;
    dict->size = 0;
}

/**
 * \brief Frees the dictionary.
 *
 * \param dict Pointer to the dictionary to free.
 */
void
freeDictionary (Dictionary *dict)
{
    size_t i;
    for (i = 0; i < dict->size; i++)
        {
            gcFree (dict->keys[i]);
            gcFree (dict->values[i]);
        }
    gcFree (dict->keys);
    gcFree (dict->values);
    dict->size = 0;
}

/**
 * \brief Adds a header to the dictionary.
 *
 * \param dict Pointer to the dictionary.
 * \param key Key of the header.
 * \param value Value of the header.
 */
void
addHeader (Dictionary *dict, const char *key, const char *value)
{
    char **new_keys;
    char **new_values;

    new_keys = (char **)gcMalloc ((dict->size + 1) * sizeof (char *));
    new_values = (char **)gcMalloc ((dict->size + 1) * sizeof (char *));

    if (new_keys == NULL || new_values == NULL)
    {
        logError("Memory allocation failed");
        return;
    }

    /* Add header logic here */
}
