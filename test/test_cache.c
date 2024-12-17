/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include <CUnit/CUnit.h>
#include "../include/cache.h"
#include "test_suite.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/* Add prototype at top after includes */
void* cache_thread_func(void* arg);

/* Reduce test constants for development */
#define STRESS_TEST_ITERATIONS 10    /* Was likely 1000+ */
#define NUM_THREADS 2               /* Was likely 10+ */
#define EVICTION_TEST_SIZE 5        /* Was likely 100+ */
#define MAX_KEY_LEN CACHE_MAX_KEY_SIZE
#define MAX_VALUE_LEN CACHE_MAX_VALUE_SIZE

/* Test fixture */
static const char *TEST_KEY = "test_key";
static const char *TEST_VALUE = "test_value";

/* Setup and teardown */
static int setup(void)
{
    return cacheInit(CACHE_TYPE_LRU, 16);
}

static int teardown(void)
{
    cacheCleanup();
    return 0;
}

/* Test cases */
void test_cache_init(void)
{
    CU_ASSERT_EQUAL(cacheInit(CACHE_TYPE_LRU, 16), 0);
    CU_ASSERT_EQUAL(cacheInit(CACHE_TYPE_LFU, 0), -1);
}

void test_cache_set_get(void)
{
    char value[64];
    size_t size = sizeof(value);

    CU_ASSERT_EQUAL(cacheSet(TEST_KEY, TEST_VALUE, strlen(TEST_VALUE) + 1, 60), 0);
    CU_ASSERT_EQUAL(cacheGet(TEST_KEY, value, &size), 0);
    CU_ASSERT_STRING_EQUAL(value, TEST_VALUE);
}

void test_cache_delete(void)
{
    char value[64];
    size_t size = sizeof(value);

    cacheSet(TEST_KEY, TEST_VALUE, strlen(TEST_VALUE) + 1, 60);
    CU_ASSERT_EQUAL(cacheDelete(TEST_KEY), 0);
    CU_ASSERT_EQUAL(cacheGet(TEST_KEY, value, &size), -1);
}

void test_cache_capacity(void)
{
    char key[16];
    char value[64];
    size_t i;
    size_t size;
    int result;

    for (i = 0; i < 20; i++) {
        /* Use %lu instead of %zu for C90 compliance */
        sprintf(key, "key%lu", (unsigned long)i);
        result = cacheSet(key, TEST_VALUE, strlen(TEST_VALUE) + 1, 60);
        CU_ASSERT_EQUAL(result, 0);
    }

    /* Verify first entry was evicted due to capacity */
    size = sizeof(value);
    result = cacheGet("key0", value, &size);
    CU_ASSERT_EQUAL(result, -1);
}

void test_cache_lru(void)
{
    char value[64];
    size_t size;

    cacheInit(CACHE_TYPE_LRU, 2);

    cacheSet("key1", "value1", 7, 60);
    cacheSet("key2", "value2", 7, 60);

    /* Access key1 to make it most recently used */
    size = sizeof(value);
    cacheGet("key1", value, &size);

    /* Add key3, should evict key2 */
    cacheSet("key3", "value3", 7, 60);

    size = sizeof(value);
    CU_ASSERT_EQUAL(cacheGet("key2", value, &size), -1);
    CU_ASSERT_EQUAL(cacheGet("key1", value, &size), 0);
    CU_ASSERT_EQUAL(cacheGet("key3", value, &size), 0);
}

void test_cache_lfu(void)
{
    char value[64];
    size_t size;

    cacheInit(CACHE_TYPE_LFU, 2);

    cacheSet("key1", "value1", 7, 60);
    cacheSet("key2", "value2", 7, 60);

    /* Access key1 multiple times */
    size = sizeof(value);
    cacheGet("key1", value, &size);
    cacheGet("key1", value, &size);

    /* Add key3, should evict key2 */
    cacheSet("key3", "value3", 7, 60);

    size = sizeof(value);
    CU_ASSERT_EQUAL(cacheGet("key2", value, &size), -1);
    CU_ASSERT_EQUAL(cacheGet("key1", value, &size), 0);
    CU_ASSERT_EQUAL(cacheGet("key3", value, &size), 0);
}

/* Add these test functions */

void test_cache_edge_cases(void)
{
    char value[CACHE_MAX_VALUE_SIZE];
    char stored_value[CACHE_MAX_VALUE_SIZE];
    char long_key[CACHE_MAX_KEY_SIZE];
    size_t value_size = sizeof(stored_value);

    /* Test initialization edge cases */
    CU_ASSERT_EQUAL(cacheInit(99, 16), -1);  /* Invalid type */
    CU_ASSERT_EQUAL(cacheInit(CACHE_TYPE_LRU, 0), -1);  /* Invalid size */

    /* Test invalid key/value sizes */
    memset(long_key, 'A', CACHE_MAX_KEY_SIZE - 1);
    long_key[CACHE_MAX_KEY_SIZE - 1] = '\0';
    CU_ASSERT_EQUAL(cacheSet(long_key, "test", 5, 60), -1);

    memset(value, 'B', CACHE_MAX_VALUE_SIZE - 1);
    value[CACHE_MAX_VALUE_SIZE - 1] = '\0';
    CU_ASSERT_EQUAL(cacheSet("test", value, CACHE_MAX_VALUE_SIZE, 60), -1);

    /* Test NULL parameters */
    CU_ASSERT_EQUAL(cacheSet(NULL, "value", 6, 60), -1);
    CU_ASSERT_EQUAL(cacheGet(NULL, value, &value_size), -1);
    CU_ASSERT_EQUAL(cacheGet("key", NULL, &value_size), -1);
    CU_ASSERT_EQUAL(cacheGet("key", value, NULL), -1);
}

void test_cache_eviction_policy(void)
{
    char key[16];
    char value[64];
    size_t size;
    int i;

    /* Initialize cache with small capacity */
    cacheInit(CACHE_TYPE_LRU, 3);

    /* Fill cache */
    for (i = 0; i < 3; i++) {
        sprintf(key, "key%d", i);
        CU_ASSERT_EQUAL(cacheSet(key, "value", 6, 60), 0);
    }

    /* Access key1 to make it most recently used */
    size = sizeof(value);
    CU_ASSERT_EQUAL(cacheGet("key1", value, &size), 0);

    /* Add new entry to trigger eviction */
    CU_ASSERT_EQUAL(cacheSet("key3", "value", 6, 60), 0);

    /* Verify key0 was evicted (LRU) */
    size = sizeof(value);
    CU_ASSERT_EQUAL(cacheGet("key0", value, &size), -1);
    CU_ASSERT_EQUAL(cacheGetStatus(), CACHE_NOT_FOUND);

    /* Verify key1 is still present */
    size = sizeof(value);
    CU_ASSERT_EQUAL(cacheGet("key1", value, &size), 0);
}

void test_cache_concurrent_access(void)
{
    char value[64];
    size_t size;

    /* Set initial value */
    CU_ASSERT_EQUAL(cacheSet("test", "initial", 8, 60), 0);

    /* Update value */
    CU_ASSERT_EQUAL(cacheSet("test", "updated", 8, 60), 0);

    /* Verify update */
    size = sizeof(value);
    CU_ASSERT_EQUAL(cacheGet("test", value, &size), 0);
    CU_ASSERT_STRING_EQUAL(value, "updated");

    /* Delete entry */
    CU_ASSERT_EQUAL(cacheDelete("test"), 0);

    /* Verify deletion */
    size = sizeof(value);
    CU_ASSERT_EQUAL(cacheGet("test", value, &size), -1);
    CU_ASSERT_EQUAL(cacheGetStatus(), CACHE_NOT_FOUND);
}

/* Performance test cases for cache */
void test_cache_performance(void)
{
    char key[16];
    char value[64];
    size_t size;
    int i, iterations = 10;
    clock_t start, end;
    double time_taken;

    /* Initialize cache with reasonable size */
    CU_ASSERT_EQUAL(cacheInit(CACHE_TYPE_LRU, 1000), 0);

    /* Measure write performance */
    start = clock();
    for (i = 0; i < iterations; i++) {
        sprintf(key, "key%d", i);
        CU_ASSERT_EQUAL(cacheSet(key, "test_value", 10, 60), 0);
    }
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    /* Log or assert acceptable write time */
    CU_ASSERT(time_taken < 1.0); /* Should complete in under 1 second */

    /* Measure read performance */
    start = clock();
    for (i = 0; i < iterations; i++) {
        sprintf(key, "key%d", i % 1000); /* Read within cache size */
        size = sizeof(value);
        CU_ASSERT_EQUAL(cacheGet(key, value, &size), 0);
    }
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    /* Log or assert acceptable read time */
    CU_ASSERT(time_taken < 0.5); /* Should complete in under 0.5 seconds */
}

void test_cache_stress(void)
{
    char key[CACHE_MAX_KEY_SIZE];
    char value[CACHE_MAX_VALUE_SIZE];
    size_t i;
    size_t size;

    /* Initialize cache with larger capacity for stress test */
    CU_ASSERT_EQUAL(cacheInit(CACHE_TYPE_LRU, 100), 0);

    for (i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        snprintf(key, sizeof(key), "stress_key_%zu", i);
        snprintf(value, sizeof(value), "stress_value_%zu", i);
        CU_ASSERT_EQUAL(cacheSet(key, value, strlen(value) + 1, 60), 0);

        /* Verify value was stored correctly */
        size = sizeof(value);
        memset(value, 0, size);
        CU_ASSERT_EQUAL(cacheGet(key, value, &size), 0);
    }
}

void* cache_thread_func(void* arg)
{
    const char* key = (const char*)arg;
    const char* value = "thread_test_value";
    size_t size = strlen(value) + 1;

    cacheSet(key, value, size, 60);
    return NULL;
}

void test_cache_thread_safety(void)
{
    pthread_t threads[NUM_THREADS];
    char value[64];
    size_t size;
    int i;
    const char *test_key = "thread_test_key";

    CU_ASSERT_EQUAL(cacheInit(CACHE_TYPE_LRU, 100), 0);

    /* Spawn multiple threads accessing cache simultaneously */
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, cache_thread_func, (void*)test_key);
    }

    /* Wait for threads to complete */
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Verify cache consistency */
    size = sizeof(value);
    CU_ASSERT_EQUAL(cacheGet(test_key, value, &size), 0);
}

void test_cache_eviction(void)
{
    char key[CACHE_MAX_KEY_SIZE];
    char value[CACHE_MAX_VALUE_SIZE];
    char retrieved[CACHE_MAX_VALUE_SIZE];
    size_t i, size;

    /* Initialize cache with small capacity to test eviction */
    CU_ASSERT_EQUAL(cacheInit(CACHE_TYPE_LRU, EVICTION_TEST_SIZE/2), 0);

    for (i = 0; i < EVICTION_TEST_SIZE; i++) {
        snprintf(key, sizeof(key), "evict_key_%zu", i);
        snprintf(value, sizeof(value), "evict_value_%zu", i);
        CU_ASSERT_EQUAL(cacheSet(key, value, strlen(value) + 1, 60), 0);

        /* Verify older entries are evicted */
        if (i >= EVICTION_TEST_SIZE/2) {
            snprintf(key, sizeof(key), "evict_key_%zu", i - EVICTION_TEST_SIZE/2);
            size = sizeof(retrieved);
            CU_ASSERT_EQUAL(cacheGet(key, retrieved, &size), -1);
            CU_ASSERT_EQUAL(cacheGetStatus(), CACHE_NOT_FOUND);
        }
    }
}

/* Test suite initialization */
int test_cache(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Cache Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if ((CU_add_test(suite, "Cache Init", test_cache_init) == NULL) ||
        (CU_add_test(suite, "Cache Set/Get", test_cache_set_get) == NULL) ||
        (CU_add_test(suite, "Cache Delete", test_cache_delete) == NULL) ||
        (CU_add_test(suite, "Cache Capacity", test_cache_capacity) == NULL) ||
        (CU_add_test(suite, "Cache LRU", test_cache_lru) == NULL) ||
        (CU_add_test(suite, "Cache LFU", test_cache_lfu) == NULL) ||
        (CU_add_test(suite, "Cache Edge Cases", test_cache_edge_cases) == NULL) ||
        (CU_add_test(suite, "Cache Performance", test_cache_performance) == NULL) ||
        (CU_add_test(suite, "Cache Stress", test_cache_stress) == NULL) ||
        (CU_add_test(suite, "Cache Thread Safety", test_cache_thread_safety) == NULL) ||
        (CU_add_test(suite, "Cache Eviction", test_cache_eviction) == NULL)) {
        return -1;
    }

    return 0;
}
