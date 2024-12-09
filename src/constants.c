/**
 * \file constants.c
 * \brief Implements functions to load constants.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#include "../include/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define CONFIG_VERSION if not defined */
#ifndef CONFIG_VERSION
#define CONFIG_VERSION 1.0
#endif

/* Function declarations */
int loadConstants(const char *filename);

typedef struct {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
} Entry;

/**
 * \brief Loads constants from a file.
 *
 * \param filename The name of the file to load constants from.
 * \return 0 on success, -1 on failure.
 */
int loadConstants(const char *filename)
{
    FILE *file;
    char buffer[256];

    file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char key[KEY_SIZE];
        char value[VALUE_SIZE];
        Entry entry;

        if (sscanf(buffer, "%s = %s", key, value) == 2)
        {
            strncpy(entry.key, key, sizeof(entry.key) - 1);
            entry.key[sizeof(entry.key) - 1] = '\0';
            strncpy(entry.value, value, sizeof(entry.value) - 1);
            entry.value[sizeof(entry.value) - 1] = '\0';
        }
    }
    fclose(file);
    return 0;
}
