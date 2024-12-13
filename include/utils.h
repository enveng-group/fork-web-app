/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

/**
 * Generates a UUID string
 * Returns: Pointer to static buffer containing UUID string
 */
char* generateUUID(void);

/**
 * Creates a directory if it doesn't exist
 * path: Path to directory
 * Returns: 0 on success, -1 on failure
 */
int createDirectoryIfNotExists(const char *path);

/**
 * Checks if a file exists
 * path: Path to file
 * Returns: 1 if exists, 0 if not
 */
int fileExists(const char *path);

/**
 * Sets file permissions
 * path: Path to file
 * mode: Permission mode (octal)
 * Returns: 0 on success, -1 on failure
 */
int setFilePermissions(const char *path, mode_t mode);

/**
 * Gets current username
 * Returns: Username string or NULL on failure
 */
char* getCurrentUsername(void);

/**
 * Validates a file path
 * path: Path to validate
 * Returns: 1 if valid, 0 if invalid
 */
int validatePath(const char *path);

/**
 * Securely zeros memory
 * ptr: Pointer to memory
 * size: Size of memory to zero
 */
void secureZeroMemory(void *ptr, size_t size);

#endif /* UTILS_H */
