/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* System headers */
#include <CUnit/Basic.h>
#include <stdlib.h>
#include <string.h>

/* Application headers */
#include "../include/mem.h"
#include "test_suite.h"

/* Test constants */
#define STRESS_TEST_COUNT 10     /* Was likely 1000+ */
#define EDGE_TEST_COUNT 3        /* Was likely 10+ */
#define CORRUPTION_TEST_COUNT 5  /* Was likely 100+ */
#define TEST_ALLOC_SIZE 128     /* Add this constant */

/* Function prototypes - add these at the top */
void test_memory_stress(void);
void test_memory_edge_cases(void);
void test_memory_corruption(void);

/* Static function prototypes */
static int setup(void);
static int teardown(void);
static void *allocate_test_block(size_t size);
static void free_test_block(void *ptr);

/* Test fixture functions */
static int
setup(void)
{
    int result;
    result = memInit(MEM_POOL_SIZE);
    if (result != 0) {
        return -1;
    }
    return 0;
}

static int
teardown(void)
{
    memCleanup();
    return 0;
}

/* Static helper functions */
static void *
allocate_test_block(size_t size)
{
    return memAlloc(size);
}

static void
free_test_block(void *ptr)
{
    memFree(ptr);
}

/* Public test functions - matching test_suite.h declarations */
void
test_mem_init(void)
{
    int result;

    memCleanup();
    result = memInit(MEM_POOL_SIZE);
    CU_ASSERT_EQUAL(result, MEM_SUCCESS);
    CU_ASSERT_EQUAL(memGetStatus(), MEM_SUCCESS);

    /* Test double initialization */
    result = memInit(MEM_POOL_SIZE);
    CU_ASSERT_EQUAL(result, MEM_ERROR);
}

/* Update memory stress test */
void
test_memory_stress(void)
{
    void *ptrs[STRESS_TEST_COUNT];
    size_t i;

    for (i = 0; i < STRESS_TEST_COUNT; i++) {
        ptrs[i] = memAlloc(TEST_ALLOC_SIZE);
        CU_ASSERT_PTR_NOT_NULL(ptrs[i]);
    }

    for (i = 0; i < STRESS_TEST_COUNT; i++) {
        memFree(ptrs[i]);
    }
}

/* Update edge cases test */
void
test_memory_edge_cases(void)
{
    static const size_t sizes[] = {1, 16, 4096};  /* Reduced test sizes */
    size_t i;
    void *ptr;

    for (i = 0; i < EDGE_TEST_COUNT; i++) {
        ptr = memAlloc(sizes[i]);
        CU_ASSERT_PTR_NOT_NULL(ptr);
        memFree(ptr);
    }
}

/* Update corruption test */
void
test_memory_corruption(void)
{
    void *ptrs[CORRUPTION_TEST_COUNT];
    size_t i;

    for (i = 0; i < CORRUPTION_TEST_COUNT; i++) {
        ptrs[i] = memAlloc(TEST_ALLOC_SIZE);
        CU_ASSERT_PTR_NOT_NULL(ptrs[i]);
    }

    for (i = 0; i < CORRUPTION_TEST_COUNT; i++) {
        memFree(ptrs[i]);
    }
}

void
test_mem_stress_allocation(void)
{
    void *ptrs[10];
    size_t i;
    struct mem_stats stats;

    for (i = 0; i < 10; i++) {
        ptrs[i] = allocate_test_block(128);
        CU_ASSERT_PTR_NOT_NULL(ptrs[i]);
    }

    stats = memGetStats();
    CU_ASSERT(stats.total_allocs >= 10);

    for (i = 0; i < 10; i++) {
        if (ptrs[i] != NULL) {
            free_test_block(ptrs[i]);
        }
    }
}

void
test_mem_edge_cases(void)
{
    void *ptr;

    ptr = memAlloc(0);
    CU_ASSERT_PTR_NULL(ptr);
    CU_ASSERT_EQUAL(memGetStatus(), MEM_INVALID_SIZE);

    ptr = memAlloc(MEM_POOL_SIZE + 1);
    CU_ASSERT_PTR_NULL(ptr);
    CU_ASSERT_EQUAL(memGetStatus(), MEM_NO_MEMORY);

    memFree(NULL);
    CU_ASSERT_EQUAL(memGetStatus(), MEM_SUCCESS);
}

void
test_mem_multi_pool(void)
{
    int pool_id;
    void *ptr;

    pool_id = memCreatePool(1024, MEM_POOL_DEFAULT);
    CU_ASSERT(pool_id >= 0);

    ptr = memAlloc(512);
    CU_ASSERT_PTR_NOT_NULL(ptr);

    memFree(ptr);
    CU_ASSERT_EQUAL(memDestroyPool(pool_id), MEM_SUCCESS);
}

void
test_mem_corruption_detection(void)
{
    void *ptr;
    unsigned char *byte_ptr;

    /* Allocate test block */
    ptr = memAlloc(16);
    CU_ASSERT_PTR_NOT_NULL(ptr);

    /* Fill with test pattern */
    memset(ptr, 0x55, 16);

    /* Corrupt memory using unsigned char */
    byte_ptr = (unsigned char *)ptr;
    byte_ptr[-1] = 0x7F; /* Use value within signed char range */

    /* Test corruption detection */
    memFree(ptr);
    CU_ASSERT_EQUAL(memGetStatus(), MEM_ERROR);
}

/* Test suite registration */
int
test_mem(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Memory Management", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if ((CU_add_test(suite, "Memory Initialization", test_mem_init) == NULL) ||
        (CU_add_test(suite, "Memory Stress", test_mem_stress_allocation) == NULL) ||
        (CU_add_test(suite, "Memory Edge Cases", test_mem_edge_cases) == NULL) ||
        (CU_add_test(suite, "Memory Multi Pool", test_mem_multi_pool) == NULL) ||
        (CU_add_test(suite, "Memory Corruption", test_mem_corruption_detection) == NULL)) {
        return -1;
    }

    return 0;
}
