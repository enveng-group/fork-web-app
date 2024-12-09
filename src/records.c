/**
 * \file records.c
 * \brief Implements record handling functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/records.h"
#include "../include/compat.h"
#include "../include/error_codes.h"
#include "../include/garbage_collector.h"
#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * \brief Initializes a RecordProject structure.
 *
 * \param project The RecordProject structure to initialize.
 */
void
initRecordProject (RecordProject *project)
{
    if (project == NULL)
        {
            return;
        }

    project->entries = NULL;
    project->entry_count = 0;
}

/**
 * \brief Frees the memory allocated for a RecordProject structure.
 *
 * \param project The RecordProject structure to free.
 */
void
freeRecordProject (RecordProject *project)
{
    if (project == NULL)
        {
            return;
        }

    if (project->entries != NULL)
        {
            free (project->entries);
        }

    project->entry_count = 0;
}

/**
 * \brief Adds an entry to a RecordProject.
 *
 * \param project Pointer to the RecordProject.
 * \param entry Pointer to the Record entry.
 * \param gc Pointer to the garbage collector.
 * \return SUCCESS on success, otherwise an error code.
 */
int
addEntry (RecordProject *project, const Record *entry, GarbageCollector *gc)
{
    Record *new_entries;

    if (project == NULL || entry == NULL || gc == NULL)
        {
            return ERROR_NULL_POINTER;
        }

    project->entries = new_entries;
    project->entries[project->entry_count] = *entry;
    project->entry_count++;

    return SUCCESS;
}

/**
 * \brief Removes an entry from a RecordProject structure by index.
 *
 * \param project The RecordProject structure.
 * \param index The index of the entry to remove.
 * \return SUCCESS on success, otherwise an error code.
 */
int
removeEntry (RecordProject *project, size_t index)
{
    if (project == NULL || index >= project->entry_count)
        {
            return ERROR_INVALID_VALUE;
        }

    for (size_t i = index; i < project->entry_count - 1; i++)
        {
            project->entries[i] = project->entries[i + 1];
        }

    Record *new_entries = realloc (project->entries, (project->entry_count - 1)
                                                         * sizeof (Record));
    if (new_entries == NULL && project->entry_count > 1)
        {
            return ERROR_MEMORY_ALLOC;
        }

    project->entries = new_entries;
    project->entry_count--;

    return SUCCESS;
}

/**
 * \brief Finds an entry in a RecordProject structure by ID.
 *
 * \param project The RecordProject structure.
 * \param id The ID of the entry to find.
 * \return Pointer to the found entry, or NULL if not found.
 */
Record *
findEntryById (const RecordProject *project, int id)
{
    if (project == NULL)
        {
            return NULL;
        }

    for (size_t i = 0; i < project->entry_count; i++)
        {
            if (project->entries[i].id == id)
                {
                    return &project->entries[i];
                }
        }

    return NULL;
}

/**
 * \brief Updates an entry in a RecordProject structure by ID.
 *
 * \param project The RecordProject structure.
 * \param id The ID of the entry to update.
 * \param new_entry The new entry data.
 * \return SUCCESS on success, otherwise an error code.
 */
int
updateEntryById (RecordProject *project, int id, const Record *new_entry)
{
    if (project == NULL || new_entry == NULL)
        {
            return ERROR_NULL_POINTER;
        }

    Record *entry = findEntryById (project, id);
    if (entry == NULL)
        {
            return ERROR_INVALID_VALUE;
        }

    *entry = *new_entry;
    return SUCCESS;
}

/**
 * \brief Prints the contents of a RecordProject structure.
 *
 * \param project The RecordProject structure to print.
 */
void
printRecordProject (const RecordProject *project)
{
    if (project == NULL)
        {
            return;
        }

    for (size_t i = 0; i < project->entry_count; i++)
        {
            printf ("Entry %zu: ID=%d, Name=%s\n", i, project->entries[i].id,
                    project->entries[i].name);
        }
}

/**
 * \brief Loads a RecordProject structure from a file.
 *
 * \param filename The name of the file to load.
 * \param project The RecordProject structure to load into.
 * \return SUCCESS on success, otherwise an error code.
 */
int
loadRecordProjectFromFile (const char *filename, RecordProject *project)
{
    if (filename == NULL || project == NULL)
        {
            return ERROR_NULL_POINTER;
        }

    FILE *file = fopen (filename, "r");
    if (file == NULL)
        {
            return ERROR_FILE_OPEN;
        }

    initRecordProject (project);

    Record entry;
    while (fscanf (file, "%d,%255s", &entry.id, entry.name) == 2)
        {
            if (addEntry (project, &entry, NULL) != SUCCESS)
                {
                    fclose (file);
                    return ERROR_MEMORY_ALLOC;
                }
        }

    fclose (file);
    return SUCCESS;
}
