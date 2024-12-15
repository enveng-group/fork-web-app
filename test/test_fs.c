#include <CUnit/CUnit.h>
#include "../include/fs.h"
#include "test_suite.h"
#include <stdlib.h>
#include <string.h>

/* Test fixture */
static const char *TEST_ROOT_PATH = "test/fs_test";
static const char *TEST_FILE_PATH = "test/fs_test/test.txt";
static const char *TEST_DATA = "Hello, World!";

/* Setup and teardown */
static int setup(void)
{
    mkdir(TEST_ROOT_PATH, 0755);
    return 0;
}

static int teardown(void)
{
    remove(TEST_FILE_PATH);
    rmdir(TEST_ROOT_PATH);
    return 0;
}

/* Test cases */
void test_fs_init_null(void)
{
    CU_ASSERT_EQUAL(fsInit(NULL), FS_ERROR);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_ERROR);
}

void test_fs_init_valid(void)
{
    CU_ASSERT_EQUAL(fsInit(TEST_ROOT_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_SUCCESS);
}

void test_fs_create_file(void)
{
    fsInit(TEST_ROOT_PATH);
    CU_ASSERT_EQUAL(fsCreateFile(TEST_FILE_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_OK);
}

void test_fs_delete_file(void)
{
    fsInit(TEST_ROOT_PATH);
    fsCreateFile(TEST_FILE_PATH);
    CU_ASSERT_EQUAL(fsDeleteFile(TEST_FILE_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_OK);
}

void test_fs_read_file(void)
{
    char buffer[MAX_FILE_SIZE];

    fsInit(TEST_ROOT_PATH);
    fsCreateFile(TEST_FILE_PATH);
    fsWriteFile(TEST_FILE_PATH, TEST_DATA, strlen(TEST_DATA));

    CU_ASSERT_EQUAL(fsReadFile(TEST_FILE_PATH, buffer, sizeof(buffer)), strlen(TEST_DATA));
    CU_ASSERT_EQUAL(fsGetStatus(), FS_OK);
    CU_ASSERT_STRING_EQUAL(buffer, TEST_DATA);
}

void test_fs_write_file(void)
{
    fsInit(TEST_ROOT_PATH);
    CU_ASSERT_EQUAL(fsWriteFile(TEST_FILE_PATH, TEST_DATA, strlen(TEST_DATA)), strlen(TEST_DATA));
    CU_ASSERT_EQUAL(fsGetStatus(), FS_OK);
}

void test_fs_invalid_operations(void)
{
    char buffer[MAX_FILE_SIZE];

    fsInit(TEST_ROOT_PATH);

    /* Test invalid file operations */
    CU_ASSERT_EQUAL(fsReadFile("nonexistent.txt", buffer, sizeof(buffer)), FS_ERROR);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_FILE_NOT_FOUND);

    CU_ASSERT_EQUAL(fsDeleteFile("nonexistent.txt"), FS_ERROR);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_PERMISSION_DENIED);
}

/* Test suite initialization */
int test_fs(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Filesystem Module Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if (
        (CU_add_test(suite, "FS Init with NULL", test_fs_init_null) == NULL) ||
        (CU_add_test(suite, "FS Init with Valid Path", test_fs_init_valid) == NULL) ||
        (CU_add_test(suite, "FS Create File", test_fs_create_file) == NULL) ||
        (CU_add_test(suite, "FS Delete File", test_fs_delete_file) == NULL) ||
        (CU_add_test(suite, "FS Read File", test_fs_read_file) == NULL) ||
        (CU_add_test(suite, "FS Write File", test_fs_write_file) == NULL) ||
        (CU_add_test(suite, "FS Invalid Operations", test_fs_invalid_operations) == NULL)
    ) {
        return -1;
    }

    return 0;
}
