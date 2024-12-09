/**
 * \file socket_module.h
 * \brief Header file for socket-related functions.
 * \author Adrian Gallo
 * \license AGPL-3.0-or-later
 */

#ifndef SOCKET_MODULE_H
#define SOCKET_MODULE_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/* Function prototypes */
const char *getServerIp(void);
int getServerPort(void);
int createAndBindSocket(void);
int acceptConnection(int sockfd);

#endif /* SOCKET_MODULE_H */
