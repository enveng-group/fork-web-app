/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/cache.c */
#include "../include/cache.h"
#include "../include/app_error.h"
#include "../include/mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

/* Keep only these constants that aren't defined elsewhere */
#define CACHE_MIN_TTL 1
#define CACHE_MAX_TTL 86400 /* 24 hours */

struct cache_entry {
    char key[CACHE_MAX_KEY_SIZE];
    void *data;
    size_t size;
    time_t expiry;
    unsigned int access_count;
    struct cache_entry *next;
    struct cache_entry *prev;
};

static struct cache_state {
    enum cache_type type;
    size_t max_entries;
    size_t current_entries;
    struct cache_entry *head;
    struct cache_entry *tail;
    pthread_mutex_t mutex;
} cache;

static enum cache_status last_status = CACHE_SUCCESS;

/* Internal helper functions */
static void updateLRU(struct cache_entry *entry);
static void updateLFU(struct cache_entry *entry);
static struct cache_entry *findEntry(const char *key);
static void removeEntry(struct cache_entry *entry);
static void evictOldest(void);

int
cacheInit(enum cache_type type, size_t max_entries)
{
    /* Validate cache type */
    if (type != CACHE_TYPE_LRU && type != CACHE_TYPE_LFU) {
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    /* Add API version check */
    if (CACHE_API_VERSION != CACHE_API_VERSION_REQUIRED) {
        last_status = CACHE_ERROR;
        return -1;
    }

    /* Validate max entries */
    if (max_entries == 0 || max_entries > CACHE_MAX_ENTRIES) {
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    if (pthread_mutex_init(&cache.mutex, NULL) != 0) {
        last_status = CACHE_ERROR;
        return -1;
    }

    cache.type = type;
    cache.max_entries = max_entries;
    cache.current_entries = 0;
    cache.head = NULL;
    cache.tail = NULL;

    last_status = CACHE_SUCCESS;
    return 0;
}

int
cacheSet(const char *key, const void *value, size_t value_size, int ttl)
{
    struct cache_entry *entry;
    struct cache_entry *new_entry;
    void *new_data;
    time_t now;
    int status;

    /* Validate input parameters more strictly */
    if (key == NULL || value == NULL) {
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    /* Check key length - must be strictly less than max size to allow for null terminator */
    if (strlen(key) >= CACHE_MAX_KEY_SIZE - 1) {
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    /* Check value size - must not exceed max size */
    if (value_size == 0 || value_size >= CACHE_MAX_VALUE_SIZE) {
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    /* Validate TTL range */
    if (ttl < CACHE_MIN_TTL || ttl > CACHE_MAX_TTL) {
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    new_entry = NULL;
    new_data = NULL;
    status = -1;

    pthread_mutex_lock(&cache.mutex);

    /* Check if key already exists */
    entry = findEntry(key);

    /* If entry exists, update it */
    if (entry != NULL) {
        new_data = malloc(value_size);
        if (new_data == NULL) {
            pthread_mutex_unlock(&cache.mutex);
            last_status = CACHE_OUT_OF_MEMORY;
            return -1;
        }

        /* Copy new data */
        memcpy(new_data, value, value_size);

        /* Free old data and update entry */
        free(entry->data);
        entry->data = new_data;
        entry->size = value_size;
        time(&now);
        entry->expiry = now + ttl;
        entry->access_count = 1;

        /* Update cache order */
        if (cache.type == CACHE_TYPE_LRU) {
            updateLRU(entry);
        } else {
            updateLFU(entry);
        }

        pthread_mutex_unlock(&cache.mutex);
        last_status = CACHE_SUCCESS;
        return 0;
    }

    /* Create new entry if doesn't exist */
    if (cache.current_entries >= cache.max_entries) {
        evictOldest();
    }

    new_entry = (struct cache_entry *)malloc(sizeof(struct cache_entry));
    if (new_entry == NULL) {
        pthread_mutex_unlock(&cache.mutex);
        last_status = CACHE_OUT_OF_MEMORY;
        return -1;
    }

    /* Allocate space for data */
    new_data = malloc(value_size);
    if (new_data == NULL) {
        free(new_entry);
        pthread_mutex_unlock(&cache.mutex);
        last_status = CACHE_OUT_OF_MEMORY;
        return -1;
    }

    /* Initialize new entry */
    strncpy(new_entry->key, key, CACHE_MAX_KEY_SIZE - 1);
    new_entry->key[CACHE_MAX_KEY_SIZE - 1] = '\0';
    memcpy(new_data, value, value_size);
    new_entry->data = new_data;
    new_entry->size = value_size;
    time(&now);
    new_entry->expiry = now + ttl;
    new_entry->access_count = 1;
    new_entry->next = NULL;
    new_entry->prev = NULL;

    /* Add to cache and update links */
    if (cache.head == NULL) {
        /* First entry */
        cache.head = new_entry;
        cache.tail = new_entry;
    } else {
        /* Add to head */
        new_entry->next = cache.head;
        cache.head->prev = new_entry;
        cache.head = new_entry;
    }

    cache.current_entries++;
    status = 0;

    /* Update cache order */
    if (cache.type == CACHE_TYPE_LRU) {
        updateLRU(new_entry);
    } else {
        updateLFU(new_entry);
    }

    pthread_mutex_unlock(&cache.mutex);
    last_status = CACHE_SUCCESS;
    return status;
}

int
cacheGet(const char *key, void *value, size_t *value_size)
{
    struct cache_entry *entry;
    time_t now;

    if (key == NULL || value == NULL || value_size == NULL) {
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    pthread_mutex_lock(&cache.mutex);

    entry = findEntry(key);
    if (entry == NULL) {
        pthread_mutex_unlock(&cache.mutex);
        last_status = CACHE_NOT_FOUND;
        return -1;
    }

    time(&now);
    if (now > entry->expiry) {
        removeEntry(entry);
        pthread_mutex_unlock(&cache.mutex);
        last_status = CACHE_NOT_FOUND;
        return -1;
    }

    if (*value_size < entry->size) {
        pthread_mutex_unlock(&cache.mutex);
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    memcpy(value, entry->data, entry->size);
    *value_size = entry->size;
    entry->access_count++;

    if (cache.type == CACHE_TYPE_LRU) {
        updateLRU(entry);
    } else {
        updateLFU(entry);
    }

    pthread_mutex_unlock(&cache.mutex);
    last_status = CACHE_SUCCESS;
    return 0;
}

int
cacheDelete(const char *key)
{
    struct cache_entry *entry;

    if (key == NULL) {
        last_status = CACHE_INVALID_PARAM;
        return -1;
    }

    pthread_mutex_lock(&cache.mutex);

    entry = findEntry(key);
    if (entry == NULL) {
        pthread_mutex_unlock(&cache.mutex);
        last_status = CACHE_NOT_FOUND;
        return -1;
    }

    removeEntry(entry);

    pthread_mutex_unlock(&cache.mutex);
    last_status = CACHE_SUCCESS;
    return 0;
}

void
cacheCleanup(void)
{
    struct cache_entry *entry;
    struct cache_entry *next;

    pthread_mutex_lock(&cache.mutex);

    entry = cache.head;
    while (entry != NULL) {
        next = entry->next;
        free(entry->data);
        free(entry);
        entry = next;
    }

    cache.head = NULL;
    cache.tail = NULL;
    cache.current_entries = 0;

    pthread_mutex_unlock(&cache.mutex);
    pthread_mutex_destroy(&cache.mutex);
}

int
cacheFlush(void)
{
    struct cache_entry *entry;
    struct cache_entry *next;

    pthread_mutex_lock(&cache.mutex);

    /* Free all entries */
    entry = cache.head;
    while (entry != NULL) {
        next = entry->next;
        free(entry->data);
        free(entry);
        entry = next;
    }

    /* Reset cache state */
    cache.head = NULL;
    cache.tail = NULL;
    cache.current_entries = 0;

    pthread_mutex_unlock(&cache.mutex);
    last_status = CACHE_SUCCESS;
    return 0;
}

size_t
cacheGetSize(void)
{
    return cache.current_entries;
}

enum cache_status
cacheGetStatus(void)
{
    return last_status;
}

static void
updateLRU(struct cache_entry *entry)
{
    if (entry == cache.head) {
        return;
    }

    if (entry->prev != NULL) {
        entry->prev->next = entry->next;
    }
    if (entry->next != NULL) {
        entry->next->prev = entry->prev;
    }
    if (entry == cache.tail) {
        cache.tail = entry->prev;
    }

    entry->prev = NULL;
    entry->next = cache.head;
    if (cache.head != NULL) {
        cache.head->prev = entry;
    }
    cache.head = entry;

    if (cache.tail == NULL) {
        cache.tail = entry;
    }
}

static void
updateLFU(struct cache_entry *entry)
{
    struct cache_entry *current;
    struct cache_entry *prev;

    if (entry == cache.head) {
        return;
    }

    current = cache.head;
    prev = NULL;

    while (current != NULL && current->access_count >= entry->access_count) {
        prev = current;
        current = current->next;
    }

    if (entry->prev != NULL) {
        entry->prev->next = entry->next;
    }
    if (entry->next != NULL) {
        entry->next->prev = entry->prev;
    }
    if (entry == cache.tail) {
        cache.tail = entry->prev;
    }

    if (prev == NULL) {
        entry->prev = NULL;
        entry->next = cache.head;
        if (cache.head != NULL) {
            cache.head->prev = entry;
        }
        cache.head = entry;
    } else {
        entry->prev = prev;
        entry->next = prev->next;
        if (prev->next != NULL) {
            prev->next->prev = entry;
        }
        prev->next = entry;
    }

    if (cache.tail == NULL) {
        cache.tail = entry;
    }
}

static struct cache_entry *
findEntry(const char *key)
{
    struct cache_entry *entry = cache.head;

    while (entry != NULL) {
        if (strncmp(entry->key, key, CACHE_MAX_KEY_SIZE) == 0) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}

static void
removeEntry(struct cache_entry *entry)
{
    if (entry->prev != NULL) {
        entry->prev->next = entry->next;
    }
    if (entry->next != NULL) {
        entry->next->prev = entry->prev;
    }
    if (entry == cache.head) {
        cache.head = entry->next;
    }
    if (entry == cache.tail) {
        cache.tail = entry->prev;
    }

    free(entry->data);
    free(entry);
    cache.current_entries--;
}

static void
evictOldest(void)
{
    struct cache_entry *entry = cache.tail;

    if (entry != NULL) {
        removeEntry(entry);
    }
}
