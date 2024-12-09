/**
 * \file data_structures.c
 * \brief Implements data structure functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/data_structures.h"
#include "../include/garbage_collector.h"
#include "../include/records.h"
#include "../include/error_codes.h"
#include <stdlib.h>
#include <string.h>

/* Function declarations */
void *gcMalloc(GarbageCollector *gc, size_t size);
void gcFree(GarbageCollector *gc, void *ptr);
void *gcRealloc(void *ptr, size_t size);
char *gcStrdup(const char *str);

/**
 * \brief Initializes the dictionary.
 *
 * \param dict Pointer to the dictionary to initialize.
 */
void initDictionary(Dictionary *dict)
{
    if (dict == NULL)
    {
        return;
    }

    dict->keys   = NULL;
    dict->values = NULL;
    dict->size   = 0;
}
/**
 * \brief Frees the dictionary.
 *
 * \param dict Pointer to the dictionary to free.
 * \param gc Pointer to the garbage collector.
 */
void freeDictionary(Dictionary *dict, GarbageCollector *gc)
{
    size_t i;

    if (dict == NULL || gc == NULL)
    {
        return;
    }

    for (i = 0; i < dict->size; i++)
    {
        gcFree(gc, dict->keys[i]);
        gcFree(gc, dict->values[i]);
    }

    gcFree(gc, dict->keys);
    gcFree(gc, dict->values);

    dict->keys   = NULL;
    dict->values = NULL;
    dict->size   = 0;
}

/**
 * \brief Adds a key-value pair to the dictionary.
 *
 * \param dict The dictionary to add the key-value pair to.
 * \param key The key to add.
 * \param value The value to add.
 */
void addKeyValuePair(Dictionary *dict, const char *key, const char *value)
{
    char **new_keys   = NULL;
    char **new_values = NULL;

    if (dict == NULL || key == NULL || value == NULL)
    {
        return;
    }

    new_keys = realloc(dict->keys, (dict->size + 1) * sizeof(char *));
    new_values = realloc(dict->values, (dict->size + 1) * sizeof(char *));

    if (new_keys == NULL || new_values == NULL)
    {
        free(new_keys);
        free(new_values);
        return;
    }

    new_keys[dict->size] = strdup(key);
    new_values[dict->size] = strdup(value);

    if (new_keys[dict->size] == NULL || new_values[dict->size] == NULL)
    {
        free(new_keys[dict->size]);
        free(new_values[dict->size]);
        return;
    }

    dict->keys   = new_keys;
    dict->values = new_values;

    dict->size++;
}

/**
 * \brief Initializes a structure by setting its memory to zero.
 *
 * \param structure The structure to initialize.
 * \param size The size of the structure.
 */
void initStructure(void *structure, size_t size)
{
    memset(structure, 0, size);
}
