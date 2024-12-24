/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* filepath: include/web_server.h */
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>

/* Constants */
#define MAX_BUFFER_SIZE 4096
#define BASE_PATH "."
#define VAR_PATH BASE_PATH "/var"
#define ETC_PATH BASE_PATH "/etc"
#define MAX_PATH 256
#define MAX_TIMESTAMP 32

/* Subpaths */
#define MAX_LOG_SIZE 10485760    /* 10MB */
#define LOG_PATH      VAR_PATH "/log"
#define REC_PATH      VAR_PATH "/records"
#define AUTH_FILE     ETC_PATH "/auth.passwd"

/* Log files */
#define AUDIT_LOG     LOG_PATH "/audit.log"
#define UPDATES_LOG   LOG_PATH "/updates.log"

/* Record files */
#define SCJV_REC     REC_PATH "/scjv.rec"
#define MS1180_REC   REC_PATH "/ms1180.rec"
#define W6946_REC    REC_PATH "/w6946.rec"

#define DEFAULT_PORT 8080
#define WWW_ROOT "./www"
#define ACTION_LOGIN "User login"
#define ACTION_LOGOUT "User logout"
#define ACTION_VIEW_PROFILE "Viewed profile"
#define ACTION_UPDATE_PROFILE "Updated profile"
#define ACTION_VIEW_PROJECT "Viewed project"
#define ACTION_MANAGE_USERS "Managed users"
#define UNUSED(x) ((void)(x))

/* Log levels */
#define LOG_ERROR   0
#define LOG_WARN    1
#define LOG_INFO    2
#define LOG_DEBUG   3

/* Basic error codes */
#define ERR_NONE     0
#define ERR_IO      -1
#define ERR_PARAM   -2
#define ERR_AUTH    -3
#define ERR_MEMORY  -4

/* Log format constants */
#define LOG_TIME_FMT "%Y-%m-%d %H:%M:%S"
#define LOG_ENTRY_FMT "%s|%-5s|%-32s|%-32s|%s\n"

/* Log rotation constants */
#define MAX_BACKUP_COUNT 3

#define REC_FIELD_SEP ':'
#define REC_LINE_MAX 1024

/* Record operation error codes */
#define REC_ERR_NONE    0
#define REC_ERR_FORMAT -1
#define REC_ERR_IO     -2
#define REC_ERR_PERM   -3
#define REC_ERR_PARAM  -4

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
int setup_server(int port);
int handle_client(int client_socket, const char *www_root);
int handle_update_user(int client_socket, const char *username, const char *fullname, const char *email, const char *project);
int handle_users_request(int client_socket);
int parse_auth_file(const char *filename, struct user_entry *entries, size_t max_entries);
int handle_create_record(int client_socket, const char *data);
int handle_record_read(const char *filename, int client_socket);
int handle_record_write(const char *filename, const char *data);
int log_message(int level, const char *username, const char *action, const char *msg);

#endif /* WEB_SERVER_H */
