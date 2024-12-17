/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/mem.c */
#include "../include/mem.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#ifdef DEBUG
#define DEBUG_LOG(x) fprintf(stderr, "[DEBUG][%s:%d] %s\n", __FILE__, __LINE__, x)
#else
#define DEBUG_LOG(x) ((void)0)
#endif

/* Structure definitions */
struct block_header {
    size_t size;
    unsigned char magic;  /* Magic number for corruption detection */
    int free;
    struct block_header *next;
    #ifdef DEBUG
    unsigned char front_guard[8];  /* Guard bytes */
    #endif
};

#define BLOCK_MAGIC 0xAA
#define FRONT_GUARD 0xFD
#define BACK_GUARD 0xBD

struct memory_pool {
    void *base;
    size_t size;
    struct block_header *first_block;
    size_t used;
    enum mem_pool_flags flags;  /* Added */
};

/* Static variables */
static struct memory_pool pools[MEM_MAX_POOLS];
static size_t pool_count = 0;
static pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;
static int initialized = 0;
static enum mem_status last_status = MEM_SUCCESS;

/* Macros */
#define ALIGN_SIZE(s) (((s) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))
#define MIN_BLOCK_SIZE (sizeof(struct block_header) + ALIGN_SIZE(MEM_BLOCK_MIN))
#define TOTAL_SIZE(s) (sizeof(struct block_header) + ALIGN_SIZE(s))

/* Memory alignment macros */
#define MEM_ALIGN_SIZE (sizeof(void *) * 2)
#define MEM_ALIGN_MASK (MEM_ALIGN_SIZE - 1)
#define MEM_ALIGN(size) (((size) + MEM_ALIGN_MASK) & ~MEM_ALIGN_MASK)
#define MEM_MIN_BLOCK_SIZE (sizeof(struct block_header) + MEM_BLOCK_MIN)

/* Add helper functions */
static int
verify_block(const struct block_header *block)
{
    if (block == NULL) {
        last_status = MEM_ERROR;
        return 0;
    }

    /* Check magic number */
    if (block->magic != BLOCK_MAGIC) {
        last_status = MEM_ERROR;
        return 0;
    }

    #ifdef DEBUG
    {
        size_t i;
        unsigned char *back_guard;
        unsigned char *user_area;

        /* Check front guard bytes */
        for (i = 0; i < sizeof(block->front_guard); i++) {
            if (block->front_guard[i] != FRONT_GUARD) {
                last_status = MEM_ERROR;
                return 0;
            }
        }

        /* Check area immediately before user data */
        user_area = (unsigned char *)block + sizeof(struct block_header);
        if (user_area[-1] != FRONT_GUARD) {
            last_status = MEM_ERROR;
            return 0;
        }

        /* Check back guard - located after user data */
        back_guard = (unsigned char *)block + sizeof(struct block_header) + block->size;
        for (i = 0; i < sizeof(block->front_guard); i++) {
            if (back_guard[i] != BACK_GUARD) {
                last_status = MEM_ERROR;
                return 0;
            }
        }
    }
    #endif

    return 1;
}

static void
init_block(struct block_header *block, size_t size)
{
    block->size = size;
    block->magic = BLOCK_MAGIC;
    block->free = 1;
    block->next = NULL;

    #ifdef DEBUG
    {
        size_t i;
        unsigned char *back_guard;

        /* Initialize front guard */
        for (i = 0; i < sizeof(block->front_guard); i++) {
            block->front_guard[i] = FRONT_GUARD;
        }

        /* Initialize back guard */
        back_guard = (unsigned char *)block + sizeof(struct block_header) + size;
        for (i = 0; i < sizeof(block->front_guard); i++) {
            back_guard[i] = BACK_GUARD;
        }
    }
    #endif
}

static int verify_pool(const struct memory_pool *pool)
{
    struct block_header *block;
    void *pool_end;

    if (pool == NULL || pool->base == NULL) {
        return 0;
    }

    pool_end = (char *)pool->base + pool->size;
    block = pool->first_block;

    while (block != NULL) {
        /* Verify block is within pool bounds */
        if ((void *)block < pool->base ||
            (void *)((char *)block + sizeof(struct block_header) + block->size) > pool_end) {
            return 0;
        }

        /* Verify block integrity */
        if (!verify_block(block)) {
            return 0;
        }

        /* Verify next pointer */
        if (block->next != NULL &&
            ((void *)block->next <= pool->base || (void *)block->next >= pool_end)) {
            return 0;
        }

        block = block->next;
    }

    return 1;
}

int
memInit(size_t pool_size)
{
    void *pool_base;
    struct block_header *first_block;
    size_t min_size;
    size_t aligned_pool_size;

    /* Validate input size */
    if (pool_size == 0 || pool_size > (size_t)-1 - sizeof(struct block_header)) {
        last_status = MEM_INVALID_SIZE;
        return -1;
    }

    /* Check if already initialized */
    if (pool_count > 0) {
        last_status = MEM_ERROR;
        return -1;
    }

    /* Calculate minimum required size */
    min_size = sizeof(struct block_header) + ALIGN_SIZE(MEM_BLOCK_MIN);
    if (pool_size < min_size) {
        pool_size = min_size;
    }

    /* Align pool size */
    aligned_pool_size = ALIGN_SIZE(pool_size);

    pthread_mutex_lock(&mem_mutex);

    /* Allocate memory pool */
    pool_base = malloc(aligned_pool_size);
    if (pool_base == NULL) {
        pthread_mutex_unlock(&mem_mutex);
        last_status = MEM_NO_MEMORY;
        return -1;
    }

    /* Initialize first block */
    first_block = (struct block_header *)pool_base;
    init_block(first_block, aligned_pool_size - sizeof(struct block_header));

    /* Initialize pool */
    pools[0].base = pool_base;
    pools[0].size = aligned_pool_size;
    pools[0].first_block = first_block;
    pools[0].used = sizeof(struct block_header);
    pools[0].flags = MEM_POOL_DEFAULT;

    pool_count = 1;
    last_status = MEM_SUCCESS;
    initialized = 1;

    pthread_mutex_unlock(&mem_mutex);
    return 0;
}

void *memAlloc(size_t size)
{
    struct block_header *block;
    struct block_header *new_block;
    size_t total_size;
    size_t remaining_size;
    size_t i;
    void *user_ptr;

    if (size == 0 || size > (size_t)-1 - sizeof(struct block_header)) {
        last_status = MEM_INVALID_SIZE;
        return NULL;
    }

    /* Align the requested size */
    size = MEM_ALIGN(size);
    total_size = size + sizeof(struct block_header);

    #ifdef DEBUG
    total_size += 2 * sizeof(((struct block_header *)0)->front_guard);
    #endif

    pthread_mutex_lock(&mem_mutex);

    if (!initialized) {
        pthread_mutex_unlock(&mem_mutex);
        last_status = MEM_ERROR;
        return NULL;
    }

    /* Search all pools */
    for (i = 0; i < pool_count; i++) {
        if (!verify_pool(&pools[i])) {
            pthread_mutex_unlock(&mem_mutex);
            last_status = MEM_ERROR;
            return NULL;
        }

        block = pools[i].first_block;

        while (block != NULL) {
            if (!verify_block(block)) {
                pthread_mutex_unlock(&mem_mutex);
                return NULL;
            }

            if (block->free && block->size >= size) {
                remaining_size = block->size - size;

                if (remaining_size >= MEM_MIN_BLOCK_SIZE) {
                    /* Split block */
                    new_block = (struct block_header *)((char *)block + total_size);
                    init_block(new_block, remaining_size - sizeof(struct block_header));
                    new_block->next = block->next;
                    block->next = new_block;
                    block->size = size;
                }

                block->free = 0;
                pools[i].used += total_size;

                user_ptr = (char *)block + sizeof(struct block_header);

                #ifdef DEBUG
                memset(user_ptr, 0xCD, size); /* Fill with pattern */
                #endif

                pthread_mutex_unlock(&mem_mutex);
                last_status = MEM_SUCCESS;
                return user_ptr;
            }
            block = block->next;
        }
    }

    pthread_mutex_unlock(&mem_mutex);
    last_status = MEM_NO_MEMORY;
    return NULL;
}

/* Update memFree to properly handle NULL ptr case */
void memFree(void *ptr)
{
    struct block_header *block;
    struct block_header *next;
    size_t i;
    void *pool_end;

    /* Set success status for NULL ptr case */
    if (ptr == NULL) {
        last_status = MEM_SUCCESS;
        return;
    }

    pthread_mutex_lock(&mem_mutex);

    if (!initialized) {
        pthread_mutex_unlock(&mem_mutex);
        last_status = MEM_ERROR;
        return;
    }

    /* Find containing pool */
    for (i = 0; i < pool_count; i++) {
        if (!verify_pool(&pools[i])) {
            pthread_mutex_unlock(&mem_mutex);
            last_status = MEM_ERROR;
            return;
        }

        pool_end = (char *)pools[i].base + pools[i].size;

        if (ptr > pools[i].base && ptr < pool_end) {
            block = (struct block_header *)((char *)ptr - sizeof(struct block_header));

            /* Enhanced corruption detection */
            if (!verify_block(block)) {
                pthread_mutex_unlock(&mem_mutex);
                last_status = MEM_ERROR;
                return;
            }

            /* Check for underflow corruption */
            if (((unsigned char *)ptr)[-1] != FRONT_GUARD) {
                pthread_mutex_unlock(&mem_mutex);
                last_status = MEM_ERROR;
                return;
            }

            block->free = 1;
            pools[i].used -= (block->size + sizeof(struct block_header));

            /* Coalesce with next block if free */
            next = block->next;
            if (next != NULL && next->free && verify_block(next)) {
                block->size += next->size + sizeof(struct block_header);
                block->next = next->next;
            }

            #ifdef DEBUG
            /* Fill freed memory with pattern */
            memset(ptr, 0xDD, block->size);
            #endif

            pthread_mutex_unlock(&mem_mutex);
            last_status = MEM_SUCCESS;
            return;
        }
    }

    pthread_mutex_unlock(&mem_mutex);
    last_status = MEM_ERROR;
}

size_t
memGetUsage(void)
{
    size_t total_used;
    size_t i;

    if (!initialized) {
        last_status = MEM_ERROR;
        return 0;
    }

    pthread_mutex_lock(&mem_mutex);

    total_used = 0;
    for (i = 0; i < pool_count; i++) {
        total_used += pools[i].used;
    }

    pthread_mutex_unlock(&mem_mutex);
    last_status = MEM_SUCCESS;
    return total_used;
}

void
memCleanup(void)
{
    size_t i;

    pthread_mutex_lock(&mem_mutex);

    for (i = 0; i < pool_count; i++) {
        free(pools[i].base);
        pools[i].base = NULL;
        pools[i].size = 0;
        pools[i].first_block = NULL;
        pools[i].used = 0;
    }
    pool_count = 0;
    last_status = MEM_SUCCESS;
    initialized = 0;

    pthread_mutex_unlock(&mem_mutex);
}

enum mem_status
memGetStatus(void)
{
    return last_status;
}

void memDumpStats(void)
{
    size_t i;
    struct block_header *block;

    for (i = 0; i < pool_count; i++) {
        printf("Pool %lu:\n", (unsigned long)i);
        printf("  Base: %p\n", pools[i].base);
        printf("  Size: %lu\n", (unsigned long)pools[i].size);
        printf("  Used: %lu\n", (unsigned long)pools[i].used);

        block = pools[i].first_block;
        while (block != NULL) {
            printf("  Block %p:\n", (void*)block);
            printf("    Size: %lu\n", (unsigned long)block->size);
            printf("    Free: %d\n", block->free);
            block = block->next;
        }
    }
}

void memVisualizeBlocks(void)
{
    size_t i;
    struct block_header *block;

    for (i = 0; i < pool_count; i++) {
        printf("Pool %lu: ", (unsigned long)i);
        block = pools[i].first_block;
        while (block != NULL) {
            printf("[%c|%lu]->", block->free ? 'F' : 'U',
                   (unsigned long)block->size);
            block = block->next;
        }
        printf("NULL\n");
    }
}

struct mem_stats
memGetStats(void)
{
    struct mem_stats stats;
    size_t i;
    struct block_header *block;

    memset(&stats, 0, sizeof(stats));

    pthread_mutex_lock(&mem_mutex);

    for (i = 0; i < pool_count; i++) {
        stats.total_size += pools[i].size;
        stats.used_size += pools[i].used;
        stats.num_pools++;

        block = pools[i].first_block;
        while (block != NULL) {
            if (!block->free) {
                stats.total_allocs++;
            }
            block = block->next;
        }
    }

    pthread_mutex_unlock(&mem_mutex);
    return stats;
}

int
memCreatePool(size_t size, enum mem_pool_flags flags)
{
    void *pool_base;
    struct block_header *first_block;
    size_t aligned_size;
    int pool_id;

    /* Validate parameters */
    if (size == 0 || pool_count >= MEM_MAX_POOLS) {
        last_status = MEM_INVALID_PARAM;
        return -1;
    }

    /* Handle flags */
    if (flags & MEM_POOL_PROTECTED) {
        /* Use mmap with PROT_READ | PROT_WRITE for protected memory */
        #ifdef _POSIX_MAPPED_FILES
        aligned_size = ALIGN_SIZE(size);
        pool_base = mmap(NULL, aligned_size,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        -1, 0);
        if (pool_base == MAP_FAILED) {
            last_status = MEM_NO_MEMORY;
            return -1;
        }
        #else
        /* Fall back to regular malloc if mmap not available */
        aligned_size = ALIGN_SIZE(size);
        pool_base = malloc(aligned_size);
        #endif
    } else {
        /* Regular pool allocation */
        aligned_size = ALIGN_SIZE(size);
        pool_base = malloc(aligned_size);
    }

    if (pool_base == NULL) {
        last_status = MEM_NO_MEMORY;
        return -1;
    }

    pthread_mutex_lock(&mem_mutex);

    /* Initialize first block */
    first_block = (struct block_header *)pool_base;
    init_block(first_block, aligned_size - sizeof(struct block_header));

    /* Initialize pool */
    pool_id = (int)pool_count;
    pools[pool_id].base = pool_base;
    pools[pool_id].size = aligned_size;
    pools[pool_id].first_block = first_block;
    pools[pool_id].used = sizeof(struct block_header);
    pools[pool_id].flags = flags;  /* Store flags */

    pool_count++;

    pthread_mutex_unlock(&mem_mutex);
    last_status = MEM_SUCCESS;
    return pool_id;
}

/* Update memDestroyPool to handle protected pools */
int
memDestroyPool(int pool_id)
{
    if (pool_id < 0 || (size_t)pool_id >= pool_count) {
        last_status = MEM_INVALID_POOL;
        return -1;
    }

    pthread_mutex_lock(&mem_mutex);

    /* Handle protected pools */
    if (pools[pool_id].flags & MEM_POOL_PROTECTED) {
        #ifdef _POSIX_MAPPED_FILES
        if (munmap(pools[pool_id].base, pools[pool_id].size) != 0) {
            pthread_mutex_unlock(&mem_mutex);
            last_status = MEM_ERROR;
            return -1;
        }
        #else
        free(pools[pool_id].base);
        #endif
    } else {
        free(pools[pool_id].base);
    }

    pools[pool_id].base = NULL;
    pools[pool_id].size = 0;
    pools[pool_id].first_block = NULL;
    pools[pool_id].used = 0;
    pools[pool_id].flags = MEM_POOL_DEFAULT;

    pthread_mutex_unlock(&mem_mutex);
    last_status = MEM_SUCCESS;
    return 0;
}
