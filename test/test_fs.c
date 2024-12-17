#include <CUnit/CUnit.h>
#include "../include/fs.h"
#include "test_suite.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Test fixture */
static const char *TEST_ROOT_PATH = "test/fs_test";
static const char *TEST_FILE_PATH = "test/fs_test/test.txt";
static const char *TEST_DATA = "Hello, World!";
static char long_path[MAX_PATH_LEN + 10];

/* Setup and teardown */
static int setup(void)
{
    if (mkdir(TEST_ROOT_PATH, (mode_t)0755) != 0) {
        return -1;
    }
    return 0;
}

static int teardown(void)
{
    if (rmdir(TEST_ROOT_PATH) != 0) {
        return -1;
    }
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
    struct stat st;

    CU_ASSERT_EQUAL(fsInit(TEST_ROOT_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsCreateFile(TEST_FILE_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_OK);

    /* Verify file exists */
    CU_ASSERT_EQUAL(stat(TEST_FILE_PATH, &st), 0);

    /* Cleanup */
    unlink(TEST_FILE_PATH);
}

void test_fs_delete_file(void)
{
    struct stat st;

    /* Create file first */
    CU_ASSERT_EQUAL(fsInit(TEST_ROOT_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsCreateFile(TEST_FILE_PATH), FS_SUCCESS);

    /* Test deletion */
    CU_ASSERT_EQUAL(fsDeleteFile(TEST_FILE_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_OK);

    /* Verify file doesn't exist */
    CU_ASSERT_EQUAL(stat(TEST_FILE_PATH, &st), -1);
}

void test_fs_read_file(void)
{
    char buffer[MAX_FILE_SIZE];

    /* Setup test file */
    CU_ASSERT_EQUAL(fsInit(TEST_ROOT_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsWriteFile(TEST_FILE_PATH, TEST_DATA, strlen(TEST_DATA)),
                    (int)strlen(TEST_DATA));

    /* Test reading */
    CU_ASSERT_EQUAL(fsReadFile(TEST_FILE_PATH, buffer, sizeof(buffer)),
                    (int)strlen(TEST_DATA));
    CU_ASSERT_EQUAL(fsGetStatus(), FS_OK);
    CU_ASSERT_NSTRING_EQUAL(buffer, TEST_DATA, strlen(TEST_DATA));

    /* Cleanup */
    unlink(TEST_FILE_PATH);
}

void test_fs_write_file(void)
{
    CU_ASSERT_EQUAL(fsInit(TEST_ROOT_PATH), FS_SUCCESS);
    CU_ASSERT_EQUAL(fsWriteFile(TEST_FILE_PATH, TEST_DATA, strlen(TEST_DATA)),
                    (int)strlen(TEST_DATA));
    CU_ASSERT_EQUAL(fsGetStatus(), FS_OK);

    /* Cleanup */
    unlink(TEST_FILE_PATH);
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

void test_fs_error_conditions(void)
{
    size_t i;

    /* Test path too long error */
    for (i = 0; i < sizeof(long_path) - 1; i++) {
        long_path[i] = 'a';
    }
    long_path[sizeof(long_path) - 1] = '\0';

    CU_ASSERT_EQUAL(fsInit(long_path), FS_ERROR);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_PATH_TOO_LONG);

    /* Test invalid path */
    CU_ASSERT_EQUAL(fsInit("/nonexistent/path"), FS_ERROR);
    CU_ASSERT_EQUAL(fsGetStatus(), FS_FILE_NOT_FOUND);
}

/* Test suite initialization */
int test_fs(void)
{
    CU_pSuite suite;

    suite = CU_add_suite("Filesystem Tests", setup, teardown);
    if (suite == NULL) {
        return -1;
    }

    if ((CU_add_test(suite, "Init with NULL", test_fs_init_null) == NULL) ||
        (CU_add_test(suite, "Init with valid path", test_fs_init_valid) == NULL) ||
        (CU_add_test(suite, "Create file", test_fs_create_file) == NULL) ||
        (CU_add_test(suite, "Delete file", test_fs_delete_file) == NULL) ||
        (CU_add_test(suite, "Read file", test_fs_read_file) == NULL) ||
        (CU_add_test(suite, "Write file", test_fs_write_file) == NULL) ||
        (CU_add_test(suite, "Error conditions", test_fs_error_conditions) == NULL)) {
        return -1;
    }

    return 0;
}
