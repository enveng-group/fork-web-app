/**
 * \file records.h
 * \brief Header file for record structures and functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef RECORDS_H
#define RECORDS_H

#include <stddef.h> /* For size_t */
#include "garbage_collector.h"

/**
 * \struct Record
 * \brief Structure to hold individual record information.
 */
typedef struct {
    int  id;
    char name[256];
} Record;

/**
 * \struct RecordProject
 * \brief Structure to hold project information for records.
 */
typedef struct {
    Record *entries;
    size_t  entry_count;
} RecordProject;

/* Function prototypes */
void initRecordProject(RecordProject *project);
void freeRecordProject(RecordProject *project);
int addEntry(RecordProject *project, const Record *entry, GarbageCollector *gc);
int removeEntry(RecordProject *project, size_t index);
Record *findEntryById(const RecordProject *project, int id);
int updateEntryById(RecordProject *project, int id, const Record *new_entry);
void printRecordProject(const RecordProject *project);
int loadRecordProjectFromFile(const char *filename, RecordProject *project);

#endif /* RECORDS_H */
