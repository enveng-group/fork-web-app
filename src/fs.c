/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* src/fs.c */
#include "../include/fs.h"
#include "../include/app_error.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/* Static variables */
static char root_directory[MAX_PATH_LEN];
static int last_status = FS_SUCCESS;

int
fsInit(const char *root_path)
{
    struct stat st;
    size_t path_len;

    if (root_path == NULL)
    {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    /* Check path length */
    path_len = strlen(root_path);
    if (path_len >= MAX_PATH_LEN)
    {
        last_status = FS_PATH_TOO_LONG;
        return FS_ERROR;
    }

    /* Verify root path exists and is accessible */
    if (stat(root_path, &st) == -1)
    {
        last_status = FS_FILE_NOT_FOUND;
        return FS_ERROR;
    }

    /* Verify it's a directory */
    if (!S_ISDIR(st.st_mode))
    {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    /* Store the root path */
    strncpy(root_directory, root_path, MAX_PATH_LEN - 1);
    root_directory[MAX_PATH_LEN - 1] = '\0';

    last_status = FS_SUCCESS;
    return FS_SUCCESS;
}

int
fsCreateFile(const char *path)
{
    int fd;

    if (path == NULL)
    {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    fd = open(path, O_CREAT | O_WRONLY, 0644);
    if (fd == -1)
    {
        last_status = FS_PERMISSION_DENIED;
        return FS_ERROR;
    }

    close(fd);
    last_status = FS_OK;
    return FS_SUCCESS;
}

int
fsDeleteFile(const char *path)
{
    if (path == NULL)
    {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    if (unlink(path) == -1)
    {
        last_status = FS_PERMISSION_DENIED;
        return FS_ERROR;
    }

    last_status = FS_OK;
    return FS_SUCCESS;
}

int
fsReadFile(const char *path, char *buffer, size_t size)
{
    int fd;
    ssize_t bytes_read;

    if (path == NULL || buffer == NULL)
    {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        last_status = FS_FILE_NOT_FOUND;
        return FS_ERROR;
    }

    bytes_read = read(fd, buffer, size);
    close(fd);

    if (bytes_read == -1)
    {
        last_status = FS_IO_ERROR;
        return FS_ERROR;
    }

    last_status = FS_OK;
    return (int)bytes_read;
}

int
fsWriteFile(const char *path, const char *data, size_t size)
{
    int fd;
    ssize_t bytes_written;

    if (path == NULL || data == NULL)
    {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        last_status = FS_PERMISSION_DENIED;
        return FS_ERROR;
    }

    bytes_written = write(fd, data, size);
    close(fd);

    if (bytes_written == -1)
    {
        last_status = FS_IO_ERROR;
        return FS_ERROR;
    }

    last_status = FS_OK;
    return (int)bytes_written;
}

enum fs_status
fsGetStatus(void)
{
    return last_status;
}
