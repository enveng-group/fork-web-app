/**
 * \file utils.h
 * \brief Header file for utility functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>  /* For size_t */

char *trimWhitespace(char *str);
int safeStringCopy(char *dest, const char *src, size_t n);
char *processString(char *str);

#endif /* UTILS_H */
