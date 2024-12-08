/**
 * \file socket_module.h
 * \brief Header file for socket creation using POSIX socket API.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef SOCKET_MODULE_H
#define SOCKET_MODULE_H

/**
 * \brief Creates and binds a UDP socket.
 *
 * \return Created socket on success, NULL on failure.
 */
int createSocket(void);

#endif /* SOCKET_MODULE_H */
