/**
 * \file compat_types.h
 * \brief Header file for compatibility types.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#ifndef COMPAT_TYPES_H
#define COMPAT_TYPES_H

#if defined(__STDC__) && __STDC_VERSION__ >= 199409L
#include <stdint.h>
typedef uint32_t custom_uint32_t;
typedef long custom_long_t;
#else
typedef unsigned long custom_uint32_t;
typedef long custom_long_t;
#endif

typedef struct
{
    unsigned long long value;
} custom_uint64_t;

typedef struct
{
    unsigned long long quot;
    unsigned long long rem;
} compat_div_long_t;

#endif /* COMPAT_TYPES_H */
