/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

#ifndef MEM_H
#define MEM_H

/* Dependencies */
#include "common.h"

/* Documentation for dependent modules:
 * - Core memory management facility
 * - Used by cache.h for memory allocation
 * - Provides memory pool and allocation tracking
 * - Thread-safe memory operations
 */

#include <sys/types.h>

/* Constants */
#define MEM_POOL_SIZE (1024 * 1024)  /* 1MB default pool size */
#define MEM_BLOCK_MIN 16             /* Minimum block size */
#define MEM_MAX_POOLS 16            /* Maximum number of memory pools */
#define MEM_MAX_BLOCKS 1024         /* Maximum blocks per pool */

/* Memory allocation status codes */
enum mem_status {
    MEM_SUCCESS = 0,
    MEM_ERROR = -1,
    MEM_NO_MEMORY = -2,
    MEM_INVALID_POOL = -3,
    MEM_INVALID_SIZE = -4,
    MEM_POOL_FULL = -5,
    MEM_INVALID_PARAM = -6,     /* Added */
    MEM_OUT_OF_MEMORY = -7      /* Added */
};

/* Memory pool flags */
enum mem_pool_flags {
    MEM_POOL_DEFAULT = 0,
    MEM_POOL_GROWABLE = 1,
    MEM_POOL_PROTECTED = 2
};

/* Memory statistics structure */
struct mem_stats {
    size_t total_size;
    size_t used_size;
    size_t peak_usage;
    size_t total_allocs;
    size_t total_frees;
    unsigned int num_pools;
};

/* Function prototypes */
int memInit(size_t pool_size);  /* Updated */
void *memAlloc(size_t size);
void *memCalloc(size_t nmemb, size_t size);
void *memRealloc(void *ptr, size_t size);
void memFree(void *ptr);
int memCreatePool(size_t size, enum mem_pool_flags flags);
int memDestroyPool(int pool_id);
struct mem_stats memGetStats(void);
size_t memGetUsage(void);       /* Added */
void memDumpStats(void);        /* Added */
void memVisualizeBlocks(void);  /* Added */
void memCleanup(void);
const char *memStatusToString(enum mem_status status);
enum mem_status memGetStatus(void);

#endif /* MEM_H */
