/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */

/* filepath: include/web_server.h */
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

/* Standard C headers */
#include <stddef.h>
#include <stdio.h>

/* System constants */
#define MAX_BUFFER_SIZE 4096
#define DEFAULT_PORT 8080
#define UNUSED(x) ((void)(x))

/* Path constants */
#define WWW_ROOT "./www"
#define AUTH_FILE "./etc/auth.passwd"
#define RECORDS_DIR "/home/appuser/fork-web-app/var/records"
#define ENDPOINT_READ "/var/records/scjv.rec"
#define OBLIGATION_NUMBER_FILE "/var/records/next_number.txt"

/* Logging constants */
#define LOG_DIR "var/log"
#define LOG_SUFFIX ".log"
#define LOG_ARCHIVE_SUFFIX ".old"
#define LOG_MAX_SIZE (10 * 1024 * 1024) /* 10MB */
#define LOG_MAX_FILES 5

/* Action types */
#define ACTION_LOGIN "User login"
#define ACTION_LOGOUT "User logout"
#define ACTION_VIEW_PROFILE "Viewed profile"
#define ACTION_UPDATE_PROFILE "Updated profile"
#define ACTION_VIEW_PROJECT "Viewed project"
#define ACTION_MANAGE_USERS "Managed users"

/* API endpoints */
#define ENDPOINT_CREATE "/create_record"
#define ENDPOINT_UPDATE "/update_record"
#define ENDPOINT_NEXT_NUMBER "/get_next_number"

/* Error codes */
#define ERR_NONE 0      /* No error */
#define ERR_AUTH -1     /* Authentication error */
#define ERR_IO -2       /* I/O error */
#define ERR_PARAM -3    /* Invalid parameter */
#define ERR_PERM -4     /* Permission denied */
#define ERR_NOTFOUND -5 /* Not found */
#define ERR_INTERNAL -6 /* Internal error */

/* Log levels */
#define LOG_NONE    0   /* No logging */
#define LOG_ERROR  -1   /* Error conditions */
#define LOG_INFO    1   /* Informational messages */
#define LOG_WARN    2   /* Warning conditions */
#define LOG_DEBUG   3   /* Debug messages */
#define LOG_FATAL  -2   /* Fatal conditions */

/* Log types */
#define LOG_AUTH    10  /* Authentication related */
#define LOG_ACCESS  11  /* Access related */
#define LOG_SYSTEM  12  /* System related */
#define LOG_AUDIT   13  /* Audit trail */
#define LOG_CONFIG  14  /* Configuration related */
#define LOG_SECURE  15  /* Security related */

/* Data structures */
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

/* Core server functions */
int setup_server(int port);
int handle_client(int client_socket, const char *www_root);

/* Authentication functions */
int check_auth(const char *username, const char *password);
void parse_query_string(const char *query, char *username, char *password);
int parse_auth_file(const char *filename, struct user_entry *entries, size_t max_entries);

/* User management functions */
int handle_users_request(int client_socket);
int handle_update_user(int client_socket, const char *username, const char *fullname,
                      const char *email, const char *project);

/* Record management functions */
int handle_create_record(int client_socket, const char *data);
int handle_update_record(int client_socket, const char *data);
int create_record_in_file(const char *data);
int update_record_in_file(FILE *fp, const char *data);
int handle_next_number(int client_socket);
int get_next_obligation_number(void);

/* Logging functions */
int log_message(int level, const char *username, const char *action, const char *msg);
int rotate_log(const char *logname);
int check_log_size(const char *filename);

#endif /* WEB_SERVER_H */
