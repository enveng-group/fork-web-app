/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* src/fs.c */
#include "../include/fs.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Static variables */
static char root_directory[MAX_PATH_LEN] = {0};
static enum fs_status last_status = FS_OK;

/* Static function prototypes */
static int validate_path(const char *path);
static void log_fs_error(const char *message);

static int
validate_path(const char *path)
{
    size_t path_len;
    size_t root_len;

    if (path == NULL) {
        return 0;
    }

    path_len = strlen(path);
    root_len = strlen(root_directory);

    if (path_len >= MAX_PATH_LEN) {
        return 0;
    }

    /* Verify path is within root directory */
    if (root_len > 0 && strncmp(path, root_directory, root_len) != 0) {
        return 0;
    }

    return 1;
}

static void
log_fs_error(const char *message)
{
    if (message != NULL) {
        errorLog(ERROR_WARNING, message);
    }
}

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
    char dir_path[MAX_PATH_LEN];
    char *last_slash;

    if (!validate_path(path)) {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    /* Check if file already exists */
    if (access(path, F_OK) == 0) {
        last_status = FS_ALREADY_EXISTS;
        return FS_ERROR;
    }

    /* Get parent directory path */
    strncpy(dir_path, path, MAX_PATH_LEN - 1);
    dir_path[MAX_PATH_LEN - 1] = '\0';

    last_slash = strrchr(dir_path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    } else {
        strcpy(dir_path, ".");
    }

    /* Check directory write permissions */
    if (access(dir_path, W_OK) == -1) {
        log_fs_error(strerror(errno));
        last_status = FS_PERMISSION_DENIED;
        return FS_ERROR;
    }

    /* Create file with proper permissions */
    fd = open(path, O_WRONLY | O_CREAT | O_EXCL,
             (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
    if (fd == -1) {
        log_fs_error(strerror(errno));
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    close(fd);
    last_status = FS_OK;
    return FS_SUCCESS;
}

int
fsDeleteFile(const char *path)
{
    if (!validate_path(path)) {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    /* Check file permissions */
    if (access(path, R_OK | W_OK) == -1) {
        log_fs_error(strerror(errno));
        last_status = FS_PERMISSION_DENIED;
        return FS_PERMISSION_DENIED;
    }

    /* Delete the file */
    if (unlink(path) == -1) {
        log_fs_error(strerror(errno));
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    last_status = FS_OK;
    return FS_SUCCESS;
}

int
fsReadFile(const char *path, char *buffer, size_t size)
{
    FILE *fp;
    size_t bytes_read;

    if (!validate_path(path) || buffer == NULL || size == 0) {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    /* Check read permissions */
    if (access(path, R_OK) == -1) {
        log_fs_error(strerror(errno));
        last_status = FS_PERMISSION_DENIED;
        return FS_PERMISSION_DENIED;
    }

    fp = fopen(path, "rb");
    if (fp == NULL) {
        log_fs_error(strerror(errno));
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    bytes_read = fread(buffer, 1, size - 1, fp);
    if (ferror(fp)) {
        log_fs_error(strerror(errno));
        fclose(fp);
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    buffer[bytes_read] = '\0';
    fclose(fp);
    last_status = FS_OK;
    return (int)bytes_read;
}

int
fsWriteFile(const char *path, const char *data, size_t size)
{
    FILE *fp;
    size_t bytes_written;
    struct stat st;

    if (!validate_path(path) || data == NULL) {
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    /* Check if file exists */
    if (stat(path, &st) == 0) {
        /* File exists - check write permissions */
        if (access(path, W_OK) == -1) {
            log_fs_error(strerror(errno));
            last_status = FS_PERMISSION_DENIED;
            return FS_PERMISSION_DENIED;
        }
    } else {
        /* File doesn't exist - check directory write permissions */
        char dir_path[MAX_PATH_LEN];
        char *last_slash;

        strncpy(dir_path, path, MAX_PATH_LEN - 1);
        dir_path[MAX_PATH_LEN - 1] = '\0';

        last_slash = strrchr(dir_path, '/');
        if (last_slash != NULL) {
            *last_slash = '\0';
        } else {
            strcpy(dir_path, ".");
        }

        if (access(dir_path, W_OK) == -1) {
            log_fs_error(strerror(errno));
            last_status = FS_PERMISSION_DENIED;
            return FS_PERMISSION_DENIED;
        }
    }

    fp = fopen(path, "wb");
    if (fp == NULL) {
        log_fs_error(strerror(errno));
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    bytes_written = fwrite(data, 1, size, fp);
    if (bytes_written != size || ferror(fp)) {
        log_fs_error(strerror(errno));
        fclose(fp);
        last_status = FS_ERROR;
        return FS_ERROR;
    }

    if (fclose(fp) != 0) {
        log_fs_error(strerror(errno));
        last_status = FS_ERROR;
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
