/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* filepath: include/web_server.h */
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stddef.h>

/* Constants */
#define MAX_BUFFER_SIZE 4096
#define AUTH_FILE "./etc/auth.passwd"
#define DEFAULT_PORT 8080
#define WWW_ROOT "./www"
#define ACTION_LOGIN "User login"
#define ACTION_LOGOUT "User logout"
#define ACTION_VIEW_PROFILE "Viewed profile"
#define ACTION_UPDATE_PROFILE "Updated profile"
#define ACTION_VIEW_PROJECT "Viewed project"
#define ACTION_MANAGE_USERS "Managed users"
#define UNUSED(x) ((void)(x))

struct user_entry {
    char username[64];
    char password[64];
    int uid;
    int gid;
    char fullname[128];
    char homedir[128];
    char shell[64];
    int is_admin;
};

/* Function prototypes */
int check_auth(const char *username, const char *password);
void parse_query_string(const char *query, char *username, char *password);
int handle_client(int client_socket, const char *www_root);
int setup_server(int port);
int handle_update_user(int client_socket, const char *username, const char *fullname, const char *email, const char *project);
int handle_users_request(int client_socket);
int parse_auth_file(const char *filename, struct user_entry *entries, size_t max_entries);

#endif /* WEB_SERVER_H */
