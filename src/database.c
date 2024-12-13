/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* database.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "../include/database.h"
#include "../include/logger.h"
#include "../include/data_structures.h"
#include "../include/http.h" /* Add this include */

#define MAX_LINE_LENGTH 4096
#define FIELD_SEPARATOR "|"

/* Static function declarations */
static void parseProjectRecord(const char *line, struct ProjectRecord *record);
static int performSearch(struct DBHandle *handle, const char *query,
                        struct ProjectRecord **records, size_t *count);

int dbInit(const char *filename, struct DBHandle *handle)
{
    if (!filename || !handle) {
        return -1;
    }

    memset(handle, 0, sizeof(struct DBHandle));
    handle->filename = strdup(filename);
    if (!handle->filename) {
        return -1;
    }

    pthread_mutex_init(&handle->mutex, NULL);

    handle->fp = fopen(filename, "r+");
    if (!handle->fp) {
        free(handle->filename);
        pthread_mutex_destroy(&handle->mutex);
        return -1;
    }

    handle->is_open = 1;
    return 0;
}

void dbCleanup(struct DBHandle *handle)
{
    if (!handle) {
        return;
    }

    pthread_mutex_lock(&handle->mutex);

    if (handle->fp) {
        fclose(handle->fp);
        handle->fp = NULL;
    }

    if (handle->filename) {
        free(handle->filename);
        handle->filename = NULL;
    }

    handle->is_open = 0;

    pthread_mutex_unlock(&handle->mutex);
    pthread_mutex_destroy(&handle->mutex);
}

int dbReadProject(struct DBHandle *handle, const char *obligation_num, struct ProjectRecord *record)
{
    char line[MAX_LINE_LENGTH];
    char *token;
    int found = 0;

    if (!handle || !handle->fp || !obligation_num || !record) {
        return -1;
    }

    pthread_mutex_lock(&handle->mutex);

    rewind(handle->fp);

    while (fgets(line, sizeof(line), handle->fp)) {
        /* Skip header lines starting with % */
        if (line[0] == '%') {
            continue;
        }

        token = strtok(line, FIELD_SEPARATOR);
        if (!token) {
            continue;
        }

        /* Check if this is the record we want */
        if (strstr(token, obligation_num)) {
            found = 1;
            strncpy(record->obligation_num, token, sizeof(record->obligation_num) - 1);

            /* Parse remaining fields */
            token = strtok(NULL, FIELD_SEPARATOR);
            if (token) strncpy(record->project_name, token, sizeof(record->project_name) - 1);

            token = strtok(NULL, FIELD_SEPARATOR);
            if (token) strncpy(record->mechanism, token, sizeof(record->mechanism) - 1);

            /* Continue parsing other fields... */
            break;
        }
    }

    pthread_mutex_unlock(&handle->mutex);
    return found ? 0 : -1;
}

int dbUpdateProject(struct DBHandle *handle, const struct ProjectRecord *record)
{
    FILE *temp;
    char temp_path[256];
    char line[MAX_LINE_LENGTH];
    int found = 0;

    if (!handle || !handle->fp || !record) {
        logError("Invalid parameters for dbUpdateProject");
        return -1;
    }

    snprintf(temp_path, sizeof(temp_path), "%s.tmp", handle->filename);
    temp = fopen(temp_path, "w");
    if (!temp) {
        logError("Cannot create temporary file");
        return -1;
    }

    pthread_mutex_lock(&handle->mutex);
    rewind(handle->fp);

    /* Copy header lines and update records */
    while (fgets(line, sizeof(line), handle->fp)) {
        if (line[0] == '%' || strstr(line, record->obligation_num) == NULL) {
            fputs(line, temp);
            continue;
        }

        /* Write updated record */
        fprintf(temp, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d\n",
                record->obligation_num,
                record->project_name,
                record->mechanism,
                record->procedure,
                record->aspect,
                record->obligation,
                record->accountability,
                record->responsibility,
                record->phase,
                record->status,
                record->due_date,
                record->close_date,
                record->recurring,
                record->inspection_required);
        found = 1;
    }

    fclose(temp);
    fclose(handle->fp);

    /* Replace original file with temporary file */
    if (rename(temp_path, handle->filename) != 0) {
        logError("Failed to update database file");
        pthread_mutex_unlock(&handle->mutex);
        return -1;
    }

    /* Reopen the database file */
    handle->fp = fopen(handle->filename, "r+");
    if (!handle->fp) {
        logError("Failed to reopen database file");
        pthread_mutex_unlock(&handle->mutex);
        return -1;
    }

    pthread_mutex_unlock(&handle->mutex);
    return found ? 0 : -1;
}

int dbListProjects(struct DBHandle *handle, struct ProjectRecord **records, size_t *count)
{
    struct ProjectRecord *temp_records;
    char line[MAX_LINE_LENGTH];
    size_t capacity = 10;
    size_t current_count = 0;

    if (!handle || !handle->fp || !records || !count) {
        logError("Invalid parameters for dbListProjects");
        return -1;
    }

    temp_records = malloc(capacity * sizeof(struct ProjectRecord));
    if (!temp_records) {
        logError("Memory allocation failed");
        return -1;
    }

    pthread_mutex_lock(&handle->mutex);
    rewind(handle->fp);

    while (fgets(line, sizeof(line), handle->fp)) {
        char *token;

        if (line[0] == '%') {
            continue;
        }

        if (current_count >= capacity) {
            struct ProjectRecord *new_records;
            capacity *= 2;
            new_records = realloc(temp_records, capacity * sizeof(struct ProjectRecord));
            if (!new_records) {
                free(temp_records);
                pthread_mutex_unlock(&handle->mutex);
                logError("Memory reallocation failed");
                return -1;
            }
            temp_records = new_records;
        }

        /* Parse record fields */
        token = strtok(line, FIELD_SEPARATOR);
        if (token) strncpy(temp_records[current_count].obligation_num, token, sizeof(temp_records[current_count].obligation_num) - 1);

        token = strtok(NULL, FIELD_SEPARATOR);
        if (token) strncpy(temp_records[current_count].project_name, token, sizeof(temp_records[current_count].project_name) - 1);

        /* Continue parsing other fields... */
        current_count++;
    }

    pthread_mutex_unlock(&handle->mutex);
    *records = temp_records;
    *count = current_count;
    return 0;
}

static void parseProjectRecord(const char *line, struct ProjectRecord *record)
{
    char *temp_line;
    char *token;

    if (!line || !record) {
        return;
    }

    temp_line = strdup(line);
    if (!temp_line) {
        return;
    }

    /* Parse fields */
    token = strtok(temp_line, FIELD_SEPARATOR);
    if (token) strncpy(record->obligation_num, token, sizeof(record->obligation_num) - 1);

    token = strtok(NULL, FIELD_SEPARATOR);
    if (token) strncpy(record->project_name, token, sizeof(record->project_name) - 1);

    token = strtok(NULL, FIELD_SEPARATOR);
    if (token) strncpy(record->mechanism, token, sizeof(record->mechanism) - 1);

    /* Add parsing for remaining fields */

    free(temp_line);
}

static int performSearch(struct DBHandle *handle, const char *query,
                        struct ProjectRecord **records, size_t *count)
{
    char line[MAX_LINE_LENGTH];
    struct ProjectRecord *temp_records;
    struct ProjectRecord *new_records;
    size_t capacity;
    size_t current_count;

    if (!handle || !handle->fp || !query || !records || !count) {
        logError("Invalid parameters for search");
        return -1;
    }

    capacity = 10;
    current_count = 0;

    temp_records = malloc(capacity * sizeof(struct ProjectRecord));
    if (!temp_records) {
        logError("Memory allocation failed");
        return -1;
    }

    pthread_mutex_lock(&handle->mutex);
    rewind(handle->fp);

    while (fgets(line, sizeof(line), handle->fp)) {
        if (line[0] == '%') {
            continue;
        }

        if (strstr(line, query)) {
            if (current_count >= capacity) {
                capacity *= 2;
                new_records = realloc(temp_records,
                                    capacity * sizeof(struct ProjectRecord));
                if (!new_records) {
                    free(temp_records);
                    pthread_mutex_unlock(&handle->mutex);
                    return -1;
                }
                temp_records = new_records;
            }

            parseProjectRecord(line, &temp_records[current_count]);
            current_count++;
        }
    }

    pthread_mutex_unlock(&handle->mutex);

    *records = temp_records;
    *count = current_count;
    return 0;
}

int dbSearchProjects(struct DBHandle *handle, const char *query,
                    struct ProjectRecord **records, size_t *count)
{
    int result;

    if (!handle || !query || !records || !count) {
        logError("Invalid parameters for dbSearchProjects");
        return -1;
    }

    result = performSearch(handle, query, records, count);
    if (result != 0) {
        logError("Search operation failed");
        return -1;
    }

    return 0;
}
