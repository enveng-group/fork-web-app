/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef FS_H
#define FS_H

/* Dependencies */
#include "common.h"
#include "app_error.h"
#include "cache.h"

/* Documentation for dependent modules:
 * - Core filesystem operations
 * - Used by cache.h for persistent storage
 * - Used by config.h for configuration files
 * - Provides POSIX-compliant file operations
 */

/* Constants */
#define FS_SUCCESS 0
#define FS_ERROR -1
#define MAX_FILE_SIZE 4096
#define MAX_PATH_LEN 4096
#define MAX_FILES_OPEN 256
#define MAX_DIR_ENTRIES 1024

/* Error codes */
#define FS_FILE_NOT_FOUND -2
#define FS_PATH_TOO_LONG -3
#define FS_NO_PERMISSION -4
#define FS_ALREADY_EXISTS -5
#define FS_NOT_DIRECTORY -6
#define FS_DIR_NOT_EMPTY -7
#define FS_TOO_MANY_FILES -8
#define FS_INVALID_PATH -9

/* File operations return codes */
enum fs_status {
    FS_OK = 0,
    FS_PERMISSION_DENIED,
    FS_IO_ERROR,
    FS_LOCK_ERROR,
    FS_DIR_ERROR,
    FS_QUOTA_ERROR,
    FS_NAME_TOO_LONG,
    FS_NO_SPACE,
    FS_INVALID_OPERATION
};

/* Function prototypes */
/* Initialization and cleanup */
int fsInit(const char *root_path);
void fsCleanup(void);

/* File operations */
int fsCreateFile(const char *path);
int fsDeleteFile(const char *path);
int fsReadFile(const char *path, char *buffer, size_t size);
int fsWriteFile(const char *path, const char *data, size_t size);
int fsTruncateFile(const char *path, size_t size);

/* Directory operations */
int fsCreateDir(const char *path);
int fsRemoveDir(const char *path);
int fsListDir(const char *path, char ***entries, size_t *count);
void fsFreeList(char **entries, size_t count);

/* File attributes */
int fsSetPermissions(const char *path, mode_t mode);
int fsGetPermissions(const char *path, mode_t *mode);
int fsGetFileSize(const char *path, size_t *size);
int fsGetModTime(const char *path, time_t *mtime);

/* File locking */
int fsLockFile(const char *path);
int fsUnlockFile(const char *path);
int fsIsLocked(const char *path);

/* Status and information */
enum fs_status fsGetStatus(void);
const char *fsGetErrorStr(enum fs_status status);
int fsGetFreeSpace(const char *path, size_t *space);

#endif /* FS_H */
