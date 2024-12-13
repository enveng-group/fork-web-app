/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include "../include/audit.h"
#include "../include/logger.h"

#define MAX_LINE_LENGTH 512
#define INITIAL_ENTRIES 32
#define DEFAULT_MODE 0640
#define DIR_MODE 0750

static FILE *audit_file = NULL;
static pthread_mutex_t audit_mutex = PTHREAD_MUTEX_INITIALIZER;
static char audit_file_path[PATH_MAX];

int auditInit(const char *path)
{
    char *last_slash;
    int result;

    if (!path) {
        logError("Audit initialization failed: NULL path provided");
        return -1;
    }

    strncpy(audit_file_path, path, sizeof(audit_file_path) - 1);
    audit_file_path[PATH_MAX - 1] = '\0';

    /* Create directory if needed */
    last_slash = strrchr(audit_file_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        result = mkdir(audit_file_path, DIR_MODE);
        if (result != 0 && errno != EEXIST) {
            logError("Failed to create audit directory: %s", audit_file_path);
            return -1;
        }
        *last_slash = '/';
    }

    /* Open audit file */
    audit_file = fopen(audit_file_path, "a+");
    if (!audit_file) {
        logError("Failed to open audit file: %s", audit_file_path);
        return -1;
    }

    /* Set file permissions */
    result = chmod(audit_file_path, DEFAULT_MODE);
    if (result != 0) {
        logError("Failed to set audit file permissions");
        fclose(audit_file);
        audit_file = NULL;
        return -1;
    }

    return 0;
}

void auditLog(const struct AuditEntry *entry)
{
    char timestamp[32];
    time_t now;
    struct tm *tm_info;

    /* Input validation */
    if (!entry || !audit_file)
    {
        logError("Invalid audit entry or audit file not initialized");
        return;
    }

    /* Format timestamp */
    now = time(NULL);
    tm_info = localtime(&now);
    if (!tm_info)
    {
        logError("Failed to get local time");
        return;
    }

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    /* Lock mutex for thread safety */
    pthread_mutex_lock(&audit_mutex);

    /* Write audit entry */
    fprintf(audit_file, "%s|%d|%s|%d|%s\n",
            timestamp,
            (int)entry->user_id,
            entry->username,
            entry->event_type,
            entry->message);

    /* Ensure data is written to disk */
    fflush(audit_file);

    /* Release mutex */
    pthread_mutex_unlock(&audit_mutex);
}

int auditGetUserHistory(const char *username, struct AuditEntry **entries, size_t *count)
{
    FILE *read_file;
    char line[MAX_LINE_LENGTH];
    char *token;
    size_t capacity;
    size_t current_count;
    struct AuditEntry *entry_array;
    struct AuditEntry temp_entry;
    int ret;

    /* Input validation */
    if (!username || !entries || !count || !audit_file) {
        return -1;
    }

    /* Lock mutex for thread safety */
    pthread_mutex_lock(&audit_mutex);

    read_file = fopen(audit_file_path, "r");
    if (!read_file) {
        pthread_mutex_unlock(&audit_mutex);
        return -1;
    }

    /* Initialize array */
    capacity = INITIAL_ENTRIES;
    current_count = 0;
    entry_array = (struct AuditEntry *)malloc(capacity * sizeof(struct AuditEntry));

    if (!entry_array) {
        fclose(read_file);
        pthread_mutex_unlock(&audit_mutex);
        return -1;
    }

    while (fgets(line, sizeof(line), read_file)) {
        /* Parse timestamp */
        token = strtok(line, "|");
        if (!token) continue;
        temp_entry.timestamp = (time_t)atol(token);

        /* Parse user_id */
        token = strtok(NULL, "|");
        if (!token) continue;
        temp_entry.user_id = (uid_t)atoi(token);

        /* Parse username */
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(temp_entry.username, token, AUDIT_USERNAME_LEN - 1);
        temp_entry.username[AUDIT_USERNAME_LEN - 1] = '\0';

        /* Check if username matches */
        if (strcmp(temp_entry.username, username) != 0) {
            continue;
        }

        /* Parse event_type */
        token = strtok(NULL, "|");
        if (!token) continue;
        temp_entry.event_type = (enum AuditEventType)atoi(token);

        /* Parse message */
        token = strtok(NULL, "\n");
        if (!token) continue;
        strncpy(temp_entry.message, token, AUDIT_MAX_MSG_LEN - 1);
        temp_entry.message[AUDIT_MAX_MSG_LEN - 1] = '\0';

        /* Resize array if needed */
        if (current_count >= capacity) {
            capacity *= 2;
            entry_array = (struct AuditEntry *)realloc(entry_array,
                                                     capacity * sizeof(struct AuditEntry));
            if (!entry_array) {
                ret = -1;
                goto cleanup;
            }
        }

        /* Add entry to array */
        entry_array[current_count++] = temp_entry;
    }

    *entries = entry_array;
    *count = current_count;
    ret = 0;

cleanup:
    fclose(read_file);
    pthread_mutex_unlock(&audit_mutex);
    return ret;
}

void auditCleanup(void)
{
    pthread_mutex_lock(&audit_mutex);

    if (audit_file != NULL)
    {
        fflush(audit_file);
        fclose(audit_file);
        audit_file = NULL;
    }

    pthread_mutex_unlock(&audit_mutex);
    pthread_mutex_destroy(&audit_mutex);
}
