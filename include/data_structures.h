/**
 * \file data_structures.h
 * \brief Header file for data structures.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h>  /* For size_t */
#include "garbage_collector.h"

/**
 * \struct Dictionary
 * \brief Dictionary structure for headers.
 */
typedef struct
{
    char **keys;   /**< Array of keys */
    char **values; /**< Array of values */
    size_t size;   /**< Number of key-value pairs */
} Dictionary;

/**
 * \struct Request
 * \brief Request structure.
 */
typedef struct
{
    Dictionary headers; /**< Dictionary of headers */
    char *url;          /**< URL of the request */
} Request;

/**
 * \struct Response
 * \brief Response structure.
 */
typedef struct
{
    int status_code; /**< Status code of the response */
    char *body;      /**< Body of the response */
} Response;

/**
 * \struct UserCredentials
 * \brief User credentials structure.
 */
typedef struct
{
    char *username; /**< Username */
    char *password; /**< Password */
} UserCredentials;

/* Function declarations */

/**
 * \brief Initializes the dictionary.
 *
 * \param dict Pointer to the dictionary to initialize.
 */
void initDictionary(Dictionary *dict);

/**
 * \brief Frees the dictionary.
 *
 * \param dict Pointer to the dictionary to free.
 * \param gc Pointer to the garbage collector.
 */
void freeDictionary(Dictionary *dict, GarbageCollector *gc);

/**
 * \brief Adds a header to the dictionary.
 *
 * \param dict Pointer to the dictionary.
 * \param key Key of the header.
 * \param value Value of the header.
 */
void addHeader(Dictionary *dict, const char *key, const char *value);

#endif /* DATA_STRUCTURES_H */
