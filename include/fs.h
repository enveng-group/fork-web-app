/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* include/fs.h */
#ifndef FS_H
#define FS_H

/* Include POSIX headers */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FS_SUCCESS 0
#define FS_ERROR -1
#define MAX_FILE_SIZE 4096
#define MAX_PATH_LEN 4096
#define FS_FILE_NOT_FOUND -2
#define FS_PATH_TOO_LONG -3

/* File operations return codes */
enum fs_status {
    FS_OK,
    FS_PERMISSION_DENIED,
    FS_IO_ERROR
};

/* Function prototypes */
int fsInit(const char *root_path);
int fsCreateFile(const char *path);
int fsDeleteFile(const char *path);
int fsReadFile(const char *path, char *buffer, size_t size);
int fsWriteFile(const char *path, const char *data, size_t size);
enum fs_status fsGetStatus(void);

#endif /* FS_H */
