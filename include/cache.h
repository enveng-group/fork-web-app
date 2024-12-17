/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef CACHE_H
#define CACHE_H

/* Dependencies */
#include "common.h"
#include "mem.h"
#include "fs.h"

/* Documentation for dependent modules:
 * - Core caching facility
 * - Used by fs.h for file caching
 * - Used by config.h for configuration caching
 * - Thread-safe cache implementation
 */

/* Constants */
#define CACHE_MAX_KEY_SIZE 128
#define CACHE_MAX_VALUE_SIZE 4096
#define CACHE_MIN_TTL 1
#define CACHE_MAX_TTL 86400 /* 24 hours */
#define CACHE_MAX_ENTRIES 1024 /* From cache.h */

/* Cache types */
enum cache_type {
    CACHE_TYPE_LRU,    /* Least Recently Used */
    CACHE_TYPE_LFU,    /* Least Frequently Used */
    CACHE_TYPE_FIFO,   /* First In First Out */
    CACHE_TYPE_CLOCK   /* Clock replacement */
};

/* Cache status codes */
enum cache_status {
    CACHE_SUCCESS = 0,
    CACHE_ERROR = -1,
    CACHE_NOT_FOUND = -2,
    CACHE_INVALID_PARAM = -3,
    CACHE_FULL = -4,
    CACHE_OUT_OF_MEMORY = -5,
    CACHE_ALREADY_EXISTS = -6,
    CACHE_LOCKED = -7
};

/* Cache statistics structure */
struct cache_stats {
    size_t hits;
    size_t misses;
    size_t evictions;
    size_t insertions;
    size_t current_size;
    size_t max_size;
    double hit_ratio;
};

/* Function prototypes */
int cacheInit(enum cache_type type, size_t max_entries);
int cacheSet(const char *key, const void *value, size_t value_size, int ttl);
int cacheGet(const char *key, void *value, size_t *value_size);
int cacheDelete(const char *key);
void cacheCleanup(void);
size_t cacheGetSize(void);
enum cache_status cacheGetStatus(void);
int cacheFlush(void);
int cacheLock(const char *key);
int cacheUnlock(const char *key);
struct cache_stats cacheGetStats(void);
int cacheSetMaxSize(size_t max_size);
int cacheSetEvictionPolicy(enum cache_type type);

#endif /* CACHE_H */
