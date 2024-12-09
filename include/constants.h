/**
 * \file constants.h
 * \brief Defines common constants used across the application.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdlib.h>
#include "env_loader.h"

#ifndef PATH_MAX
#define PATH_MAX (getenv("PATH_MAX") ? atoi(getenv("PATH_MAX")) : 4096)
#endif

#define SERVER_IP_SIZE       16
#define SSL_CERT_FILE_SIZE   256
#define SSL_KEY_FILE_SIZE    256
#define KEY_SIZE             256
#define VALUE_SIZE           256
#define IP_SIZE              16
#define MODE_SIZE            32
#define NAME_SIZE            64
#define PATH_SIZE            256
#define MAX_ENTRIES          100
#define SERVER_PORT_SIZE     6


#endif /* CONSTANTS_H */
