/*
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
/* filepath: src/web_server.c */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <pwd.h>
#include <time.h>
#include <sys/time.h>
#include "../include/web_server.h"
#include <errno.h>
#include <dirent.h>
#include <stdint.h>

/* Handler function prototypes with unused attribute */
static int handle_update_user(int client_socket, const char *uri)
    __attribute__((unused));

static int handle_delete(int client_socket, const char *uri)
    __attribute__((unused));

static int handle_users(int client_socket)
    __attribute__((unused));

static int handle_register(int client_socket, const char *uri)
    __attribute__((unused));

int update_user_profile(const char *username, const char *fullname,
                       const char *email, const char *project);

/* HTTP method enum */
enum http_method {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_UNKNOWN
};

/* Route structure */
struct route {
    enum http_method method;
    const char *path;
    int (*handler)(int, const char *);
};

/* HTTP method string to enum converter */
static enum http_method
get_http_method(const char *method_str)
{
    if (strcmp(method_str, "GET") == 0) return HTTP_GET;
    if (strcmp(method_str, "POST") == 0) return HTTP_POST;
    if (strcmp(method_str, "PUT") == 0) return HTTP_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_DELETE;
    return HTTP_UNKNOWN;
}

/* Route table */
static const struct route routes[] = {
    {HTTP_PUT, "/user/update", handle_update_user},
    {HTTP_DELETE, "/user/delete", handle_delete},
    {HTTP_GET, "/users", handle_users},
    {HTTP_POST, "/register", handle_register},
    /* Add more routes as needed */
};

#define HTTP_404_RESPONSE \
    "HTTP/1.0 404 Not Found\r\n" \
    "Content-Type: text/html\r\n" \
    "Content-Length: 130\r\n" \
    "\r\n" \
    "<html>\r\n" \
    "<head><title>404 Not Found</title></head>\r\n" \
    "<body>\r\n" \
    "<h1>404 Not Found</h1>\r\n" \
    "<p>The requested resource was not found.</p>\r\n" \
    "</body>\r\n" \
    "</html>\r\n"

/* Send 404 response */
static int
send_404(int client_socket)
{
    ssize_t bytes_sent;

    bytes_sent = write(client_socket, HTTP_404_RESPONSE,
                      sizeof(HTTP_404_RESPONSE) - 1);

    if (bytes_sent < 0) {
        return -1;
    }

    return 0;
}

/* Route handler */
int
handle_request(int client_socket, const char *method_str, const char *uri)
{
    size_t i;
    enum http_method method;
    const size_t num_routes = sizeof(routes) / sizeof(routes[0]);

    if (method_str == NULL || uri == NULL) {
        return send_404(client_socket);
    }

    method = get_http_method(method_str);
    if (method == HTTP_UNKNOWN) {
        return send_404(client_socket);
    }

    /* Find matching route */
    for (i = 0; i < num_routes; i++) {
        if (method == routes[i].method && strcmp(uri, routes[i].path) == 0) {
            return routes[i].handler(client_socket, uri);
        }
    }

    /* No matching route found */
    return send_404(client_socket);
}

int
validate_email(const char *email)
{
    const char *at;
    const char *dot;
    size_t local_part_len;
    size_t domain_len;
    size_t len;

    /* Check for NULL pointer */
    if (email == NULL) {
        errno = EINVAL;
        return 0;
    }

    /* Check overall length constraints */
    len = strlen(email);
    if (len < 3 || len > 254) {
        return 0;
    }

    /* Find @ symbol position */
    at = strchr(email, '@');
    if (at == NULL || at == email) {
        return 0;
    }

    /* Check local part length */
    local_part_len = (size_t)(at - email);
    if (local_part_len > 64) {
        return 0;
    }

    /* Find last dot in domain part */
    dot = strrchr(at + 1, '.');
    if (dot == NULL || dot == at + 1) {
        return 0;
    }

    /* Check domain length */
    domain_len = len - local_part_len - 1; /* -1 for @ */
    if (domain_len > 255 || domain_len < 3) {
        return 0;
    }

    /* Domain must end with at least 2 chars after last dot */
    if (strlen(dot + 1) < 2) {
        return 0;
    }

    return 1;
}

int
validate_project(const char *project)
{
    const char *valid_projects[] = {
        "SCJV - Pilbara Ports",
        "W6946",
        "MS1180",
        NULL
    };
    int i;

    if (!project) {
        return 0;
    }

    for (i = 0; valid_projects[i] != NULL; i++) {
        if (strcmp(project, valid_projects[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

static int
parse_request_line(FILE *client_stream, char *method, char *uri)
{
    char line[MAX_BUFFER_SIZE];
    char *token;
    char *saveptr;

    if (fgets(line, sizeof(line), client_stream) == NULL) {
        return -1;
    }

    /* Parse method */
    token = strtok_r(line, " ", &saveptr);
    if (token == NULL) {
        return -1;
    }
    strncpy(method, token, MAX_METHOD_LEN - 1);
    method[MAX_METHOD_LEN - 1] = '\0';

    /* Parse URI */
    token = strtok_r(NULL, " ", &saveptr);
    if (token == NULL) {
        return -1;
    }
    strncpy(uri, token, MAX_URI_LEN - 1);
    uri[MAX_URI_LEN - 1] = '\0';

    return 0;
}

static const char *
get_mime_type(const char *filepath)
{
    const char *ext;

    /* Find the last dot in filepath */
    ext = strrchr(filepath, '.');
    if (ext == NULL) {
        return MIME_DEFAULT;
    }

    /* Move past the dot */
    ext++;

    /* Compare extensions */
    if (strcmp(ext, "html") == 0) {
        return MIME_HTML;
    }
    if (strcmp(ext, "css") == 0) {
        return MIME_CSS;
    }
    if (strcmp(ext, "js") == 0) {
        return MIME_JS;
    }
    if (strcmp(ext, "txt") == 0 || strcmp(ext, "log") == 0) {
        return MIME_PLAIN;
    }

    return MIME_DEFAULT;
}

ssize_t
serve_file(int client_socket, const char *uri)
{
    char filepath[MAX_PATH_LEN];
    const char *clean_uri;
    char *query_pos;
    char uri_copy[MAX_URI_LEN];
    char buffer[MAX_BUFFER_SIZE];
    FILE *file;
    struct stat st;
    size_t bytes_read;
    ssize_t total_bytes;
    ssize_t write_result;
    const char *mime_type;

    /* Make a copy of URI for manipulation */
    strncpy(uri_copy, uri, MAX_URI_LEN - 1);
    uri_copy[MAX_URI_LEN - 1] = '\0';

    /* Remove query string if present */
    query_pos = strchr(uri_copy, '?');
    if (query_pos != NULL) {
        *query_pos = '\0';
    }

    /* Basic URI sanitization */
    clean_uri = (*uri_copy == '/') ? uri_copy + 1 : uri_copy;
    if (*clean_uri == '\0') {
        clean_uri = "index.html";
    }

    /* Build filepath with WWW_ROOT for HTML files */
    if (strstr(clean_uri, ".html") != NULL) {
        snprintf(filepath, sizeof(filepath), "%s/%s", WWW_ROOT, clean_uri);
    }
    /* Handle .rec files */
    else if (strstr(clean_uri, ".rec") != NULL) {
        snprintf(filepath, sizeof(filepath), "var/db/%s", clean_uri);
    }
    /* Handle audit.log */
    else if (strcmp(clean_uri, "audit.log") == 0) {
        snprintf(filepath, sizeof(filepath), "var/log/%s", clean_uri);
    }
    /* Default to WWW_ROOT */
    else {
        snprintf(filepath, sizeof(filepath), "%s/%s", WWW_ROOT, clean_uri);
    }

    total_bytes = 0;

    /* Debug logging */
    fprintf(stderr, "Accessing file: %s\n", filepath);

    /* Check if file exists */
    if (stat(filepath, &st) < 0) {
        dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
        return -1;
    }

    /* Open and serve the file */
    file = fopen(filepath, "rb");
    if (file == NULL) {
        dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
        return -1;
    }

    /* Get MIME type before sending headers */
    mime_type = get_mime_type(filepath);

    /* Send headers with content type */
    dprintf(client_socket,
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: %s\r\n"
            "%s\r\n\r\n",
            mime_type, SERVER_STRING);

    /* Send file content */
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write_result = write(client_socket, buffer, bytes_read);
        if (write_result < 0) {
            fclose(file);
            return -1;
        }
        total_bytes += write_result;
        if (total_bytes < 0) { /* Check for overflow */
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return total_bytes;
}

int
setup_server(int port)
{
    struct sockaddr_in server_addr;
    int sock_fd;
    int optval = 1;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        return -1;
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((uint16_t)port);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sock_fd);
        return -1;
    }

    if (listen(sock_fd, 10) < 0) {
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

static int
check_auth(const char *username, const char *password)
{
    FILE *passwd_file;
    char line[MAX_BUFFER_SIZE];
    char *token;
    char *saveptr;
    char *user;
    char *pass;

    passwd_file = fopen("etc/auth.passwd", "r");
    if (passwd_file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), passwd_file) != NULL) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        /* Get username */
        token = strtok_r(line, ":", &saveptr);
        if (token == NULL) {
            continue;
        }
        user = token;

        /* Get password */
        token = strtok_r(NULL, ":", &saveptr);
        if (token == NULL) {
            continue;
        }
        pass = token;

        /* Check credentials */
        if (strcmp(username, user) == 0 && strcmp(password, pass) == 0) {
            fclose(passwd_file);
            return 1;
        }
    }

    fclose(passwd_file);
    return 0;
}

static int
add_user(const char *username, const char *password)
{
    FILE *passwd_file;
    char line[MAX_BUFFER_SIZE];
    int uid;
    int max_uid;

    /* Basic validation */
    if (username == NULL || password == NULL ||
        *username == '\0' || *password == '\0') {
        return -1;
    }

    /* Check if user already exists */
    if (check_auth(username, "")) {
        return -1;
    }

    /* Find highest UID */
    max_uid = 1000; /* Start at 1000 for regular users */
    passwd_file = fopen("etc/auth.passwd", "r");
    if (passwd_file != NULL) {
        while (fgets(line, sizeof(line), passwd_file) != NULL) {
            char *token;
            char *saveptr;
            int current_uid;

            if (line[0] == '#' || line[0] == '\n') {
                continue;
            }

            /* Skip to UID field (3rd field) */
            token = strtok_r(line, ":", &saveptr);
            token = strtok_r(NULL, ":", &saveptr);
            token = strtok_r(NULL, ":", &saveptr);

            if (token != NULL) {
                current_uid = atoi(token);
                if (current_uid > max_uid) {
                    max_uid = current_uid;
                }
            }
        }
        fclose(passwd_file);
    }

    /* Append new user */
    passwd_file = fopen("etc/auth.passwd", "a");
    if (passwd_file == NULL) {
        return -1;
    }

    uid = max_uid + 1;
    fprintf(passwd_file, "%s:%s:%d:%d:%s:/home/%s:/bin/sh\n",
            username, password, uid, uid, "User", username);

    fclose(passwd_file);
    return 0;
}

static int
ensure_audit_log(void)
{
    struct stat st;
    FILE *audit_file;

    /* Check if audit log exists */
    if (stat(AUDIT_LOG_PATH, &st) == 0) {
        /* Verify permissions */
        if ((st.st_mode & 0777) != 0640) {
            if (chmod(AUDIT_LOG_PATH, 0640) != 0) {
                return -1;
            }
        }
        return 0;
    }

    /* Create audit log if it doesn't exist */
    audit_file = fopen(AUDIT_LOG_PATH, "a");
    if (audit_file == NULL) {
        return -1;
    }

    fclose(audit_file);

    /* Set permissions */
    if (chmod(AUDIT_LOG_PATH, 0640) != 0) {
        return -1;
    }

    return 0;
}

static int
get_file_size(const char *path)
{
    struct stat st;

    if (stat(path, &st) == 0) {
        return (int)st.st_size;
    }
    return -1;
}

static int
cleanup_old_logs(void)
{
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATH_LEN];
    int log_count = 0;

    dir = opendir(LOG_DIR);
    if (dir == NULL) {
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "audit.log") != NULL) {
            log_count++;
        }
    }

    if (log_count > MAX_LOG_FILES) {
        while (log_count > MAX_LOG_FILES) {
            snprintf(path, sizeof(path),
                    AUDIT_LOG_PATTERN, log_count - 1);
            if (unlink(path) == 0) {
                log_count--;
            }
        }
    }

    closedir(dir);
    return 0;
}

static int
rotate_logs(void)
{
    char old_log[MAX_PATH_LEN];
    char new_log[MAX_PATH_LEN];
    int i;

    /* First cleanup old logs */
    if (cleanup_old_logs() != 0) {
        return -1;
    }

    /* Then rotate existing logs */
    for (i = MAX_LOG_FILES - 1; i > 0; i--) {
        snprintf(old_log, sizeof(old_log),
                AUDIT_LOG_PATTERN, i - 1);
        snprintf(new_log, sizeof(new_log),
                AUDIT_LOG_PATTERN, i);
        rename(old_log, new_log);
    }

    rename(AUDIT_LOG_PATH, new_log);
    return 0;
}

static int
log_audit_event(const char *username, const char *action)
{
    FILE *audit_file;
    time_t now;
    struct tm *tm_info;
    char timestamp[32];
    char audit_msg[MAX_AUDIT_MSG];
    int log_size;

    /* Ensure audit log exists with correct permissions */
    if (ensure_audit_log() != 0) {
        return -1;
    }

    /* Check if rotation needed */
    log_size = get_file_size(AUDIT_LOG_PATH);
    if (log_size > MAX_LOG_SIZE) {
        if (rotate_logs() != 0) {
            return -1;
        }
    }

    /* Get current time */
    time(&now);
    tm_info = localtime(&now);
    if (tm_info == NULL) {
        return -1;
    }

    /* Format timestamp */
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    /* Open audit log file */
    audit_file = fopen(AUDIT_LOG_PATH, "a");
    if (audit_file == NULL) {
        return -1;
    }

    /* Write audit entry */
    snprintf(audit_msg, sizeof(audit_msg),
             "%s|%s|%s\n", timestamp, username, action);

    if (fputs(audit_msg, audit_file) == EOF) {
        fclose(audit_file);
        return -1;
    }

    fclose(audit_file);
    return 0;
}

static int
handle_auth(int client_socket, const char *uri)
{
    char username[MAX_BUFFER_SIZE] = {0};
    char password[MAX_BUFFER_SIZE] = {0};
    char *query;
    char *token;
    char *saveptr;

    /* Extract query string */
    query = strchr(uri, '?');
    if (query == NULL) {
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }
    query++;

    /* Parse query parameters */
    token = strtok_r(query, "&", &saveptr);
    while (token != NULL) {
        if (strncmp(token, "username=", 9) == 0) {
            strncpy(username, token + 9, sizeof(username) - 1);
        } else if (strncmp(token, "password=", 9) == 0) {
            strncpy(password, token + 9, sizeof(password) - 1);
        }
        token = strtok_r(NULL, "&", &saveptr);
    }

    /* Validate credentials */
    if (check_auth(username, password)) {
        log_audit_event(username, "login");
        return set_session_cookie(client_socket, username);
    }

    dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n\r\n");
    return -1;
}

static int
handle_register(int client_socket, const char *uri)
{
    char username[MAX_BUFFER_SIZE] = {0};
    char password[MAX_BUFFER_SIZE] = {0};
    char *query;
    char *token;
    char *saveptr;

    /* Extract query string */
    query = strchr(uri, '?');
    if (query == NULL) {
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }
    query++;

    /* Parse query parameters */
    token = strtok_r(query, "&", &saveptr);
    while (token != NULL) {
        if (strncmp(token, "username=", 9) == 0) {
            strncpy(username, token + 9, sizeof(username) - 1);
        } else if (strncmp(token, "password=", 9) == 0) {
            strncpy(password, token + 9, sizeof(password) - 1);
        }
        token = strtok_r(NULL, "&", &saveptr);
    }

    /* Add user */
    if (add_user(username, password) == 0) {
        log_audit_event(username, "register");
        dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
        return 0;
    } else {
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }
}

int
update_user_profile(const char *username, const char *fullname,
                   const char *email, const char *project)
{
    FILE *passwd_file;
    FILE *temp_file;
    char line[MAX_BUFFER_SIZE];
    char temp_path[MAX_PATH_LEN];
    int found = 0;

    /* Input validation */
    if (!username || !fullname || !email || !project ||
        *username == '\0' || *fullname == '\0') {
        return -1;
    }

    /* Validate email and project */
    if (!validate_email(email) || !validate_project(project)) {
        return -1;
    }

    /* Create temp file path */
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", "etc/auth.passwd");

    passwd_file = fopen("etc/auth.passwd", "r");
    if (!passwd_file) {
        return -1;
    }

    temp_file = fopen(temp_path, "w");
    if (!temp_file) {
        fclose(passwd_file);
        return -1;
    }

    /* Process file line by line */
    while (fgets(line, sizeof(line), passwd_file)) {
        char *token;
        char *saveptr;
        char modified_line[MAX_BUFFER_SIZE];

        /* Copy comments as-is */
        if (line[0] == '#') {
            fputs(line, temp_file);
            continue;
        }

        /* Parse username */
        token = strtok_r(line, ":", &saveptr);
        if (!token) {
            continue;
        }

        if (strcmp(token, username) == 0) {
            /* Found user - update fields but preserve password, UID, GID */
            char *password = strtok_r(NULL, ":", &saveptr);
            char *uid = strtok_r(NULL, ":", &saveptr);
            char *gid = strtok_r(NULL, ":", &saveptr);

            if (!password || !uid || !gid) {
                continue;
            }

            /* Format updated line */
            snprintf(modified_line, sizeof(modified_line),
                    "%s:%s:%s:%s:%s:/home/%s:/bin/sh\n",
                    username, password, uid, gid,
                    fullname, username);

            fputs(modified_line, temp_file);
            found = 1;
        } else {
            /* Not the target user - copy line as-is */
            fputs(line, temp_file);
        }
    }

    fclose(passwd_file);
    fclose(temp_file);

    /* Only update if user was found */
    if (found) {
        if (rename(temp_path, "etc/auth.passwd") == 0) {
            return 0;
        }
    }

    unlink(temp_path);
    return -1;
}

static int
handle_profile(int client_socket, const char *uri)
{
    char username[MAX_BUFFER_SIZE] = {0};
    char fullname[MAX_BUFFER_SIZE] = {0};
    char email[MAX_BUFFER_SIZE] = {0};
    char project[MAX_BUFFER_SIZE] = {0};
    char *query;
    char *token;
    char *saveptr;

    /* Extract query string */
    query = strchr(uri, '?');
    if (query == NULL) {
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }
    query++;

    /* Parse query parameters */
    token = strtok_r(query, "&", &saveptr);
    while (token != NULL) {
        if (strncmp(token, "username=", 9) == 0) {
            strncpy(username, token + 9, sizeof(username) - 1);
        } else if (strncmp(token, "fullname=", 9) == 0) {
            strncpy(fullname, token + 9, sizeof(fullname) - 1);
        } else if (strncmp(token, "email=", 6) == 0) {
            strncpy(email, token + 6, sizeof(email) - 1);
        } else if (strncmp(token, "project=", 8) == 0) {
            strncpy(project, token + 8, sizeof(project) - 1);
        }
        token = strtok_r(NULL, "&", &saveptr);
    }

    /* Update auth.passwd */
    if (update_user_profile(username, fullname, email, project) == 0) {
        /* Add audit log entry */
        log_audit_event(username, "profile_update");
        dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
        return 0;
    }

    dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
    return -1;
}

static int
handle_users(int client_socket)
{
    FILE *passwd_file;
    char line[MAX_BUFFER_SIZE];
    char *token;
    char *saveptr;
    char users_json[MAX_BUFFER_SIZE * 8] = "["; /* Start JSON array */
    int first = 1;

    passwd_file = fopen("etc/auth.passwd", "r");
    if (passwd_file == NULL) {
        dprintf(client_socket,
                "HTTP/1.0 500 Internal Server Error\r\n\r\n");
        return -1;
    }

    /* Parse passwd file and build JSON */
    while (fgets(line, sizeof(line), passwd_file)) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        /* Add comma between objects */
        if (!first) {
            strcat(users_json, ",");
        }
        first = 0;

        /* Parse line fields */
        token = strtok_r(line, ":", &saveptr);
        if (token) { /* username */
            strcat(users_json, "{\"username\":\"");
            strcat(users_json, token);
            strcat(users_json, "\",");

            token = strtok_r(NULL, ":", &saveptr); /* skip password */
            token = strtok_r(NULL, ":", &saveptr); /* skip uid */
            token = strtok_r(NULL, ":", &saveptr); /* skip gid */

            token = strtok_r(NULL, ":", &saveptr); /* fullname */
            if (token) {
                strcat(users_json, "\"fullname\":\"");
                strcat(users_json, token);
                strcat(users_json, "\"}");
            }
        }
    }
    strcat(users_json, "]");

    fclose(passwd_file);

    /* Send JSON response */
    dprintf(client_socket,
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n%s", users_json);
    return 0;
}

static int
handle_delete(int client_socket, const char *uri)
{
    FILE *passwd_file;
    FILE *temp_file;
    char *query;
    char *token;
    char *saveptr;
    char line[MAX_BUFFER_SIZE];
    char temp_path[MAX_PATH_LEN];
    char username[MAX_BUFFER_SIZE] = {0};
    int found = 0;

    /* Extract username from query string */
    query = strchr(uri, '?');
    if (query == NULL) {
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }
    query++;

    token = strtok_r(query, "=", &saveptr);
    if (token && strcmp(token, "username") == 0) {
        token = strtok_r(NULL, "&", &saveptr);
        if (token) {
            strncpy(username, token, sizeof(username) - 1);
        }
    }

    if (username[0] == '\0') {
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }

    /* Create temp file */
    snprintf(temp_path, sizeof(temp_path), "etc/auth.passwd.tmp");

    passwd_file = fopen("etc/auth.passwd", "r");
    temp_file = fopen(temp_path, "w");

    if (!passwd_file || !temp_file) {
        if (passwd_file) fclose(passwd_file);
        if (temp_file) fclose(temp_file);
        dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
        return -1;
    }

    /* Copy all lines except deleted user */
    while (fgets(line, sizeof(line), passwd_file)) {
        if (line[0] == '#' || line[0] == '\n') {
            fputs(line, temp_file);
            continue;
        }

        token = strtok_r(line, ":", &saveptr);
        if (token && strcmp(token, username) == 0) {
            found = 1;
            continue; /* Skip this line */
        }

        fputs(line, temp_file);
    }

    fclose(passwd_file);
    fclose(temp_file);

    if (!found) {
        unlink(temp_path);
        dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
        return -1;
    }

    /* Replace original with temp file */
    if (rename(temp_path, "etc/auth.passwd") != 0) {
        unlink(temp_path);
        dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
        return -1;
    }

    /* Log the deletion */
    log_audit_event(username, "deleted");

    dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
    return 0;
}

static int
handle_update_user(int client_socket, const char *uri)
{
    char username[MAX_BUFFER_SIZE] = {0};
    char fullname[MAX_BUFFER_SIZE] = {0};
    char email[MAX_BUFFER_SIZE] = {0};
    char project[MAX_BUFFER_SIZE] = {0};
    char *query;
    char *token;
    char *saveptr;

    /* Extract query string */
    query = strchr(uri, '?');
    if (query == NULL) {
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }
    query++;

    /* Parse query parameters */
    token = strtok_r(query, "&", &saveptr);
    while (token != NULL) {
        if (strncmp(token, "username=", 9) == 0) {
            strncpy(username, token + 9, sizeof(username) - 1);
        } else if (strncmp(token, "fullname=", 9) == 0) {
            strncpy(fullname, token + 9, sizeof(fullname) - 1);
        } else if (strncmp(token, "email=", 6) == 0) {
            strncpy(email, token + 6, sizeof(email) - 1);
        } else if (strncmp(token, "project=", 8) == 0) {
            strncpy(project, token + 8, sizeof(project) - 1);
        }
        token = strtok_r(NULL, "&", &saveptr);
    }

    /* Update profile */
    if (update_user_profile(username, fullname, email, project) == 0) {
        log_audit_event(username, "profile_updated");
        dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
        return 0;
    }

    dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
    return -1;
}

int
set_session_cookie(int client_socket, const char *username)
{
    time_t expiry;
    char cookie_header[MAX_BUFFER_SIZE];

    if (!username) {
        return -1;
    }

    /* Set expiry to current time + timeout */
    expiry = time(NULL) + SESSION_TIMEOUT;

    /* Format cookie header */
    snprintf(cookie_header, sizeof(cookie_header),
            "Set-Cookie: username=%s; Path=/; Max-Age=%d\r\n",
            username, SESSION_TIMEOUT);

    /* Send cookie header */
    dprintf(client_socket, "HTTP/1.0 200 OK\r\n%s\r\n", cookie_header);

    return 0;
}

/* Remove admin checks */
int
handle_client(int client_socket, const char *root_dir)
{
    char method[MAX_METHOD_LEN];
    char uri[MAX_URI_LEN];
    char line[MAX_BUFFER_SIZE];
    char cookie_header[MAX_COOKIE_LEN] = {0};
    FILE *client_stream;
    int result;

    (void)root_dir;

    client_stream = fdopen(client_socket, "r");
    if (client_stream == NULL) {
        return -1;
    }

    /* Read request line */
    if (parse_request_line(client_stream, method, uri) < 0) {
        fclose(client_stream);
        return -1;
    }

    /* Read headers to find cookie */
    while (fgets(line, sizeof(line), client_stream)) {
        /* Check for end of headers */
        if (line[0] == '\r' || line[0] == '\n') {
            break;
        }

        /* Look for Cookie header */
        if (strncmp(line, "Cookie:", 7) == 0) {
            strncpy(cookie_header, line + 7, sizeof(cookie_header) - 1);
            break;
        }
    }

    result = -1;
    if (strcmp(method, "GET") == 0) {
        /* Allow auth and register without cookie */
        if (strncmp(uri, "/auth", 5) == 0 ||
            strncmp(uri, "/register", 9) == 0 ||
            strcmp(uri, "/index.html") == 0 ||
            strcmp(uri, "/") == 0) {
            result = handle_auth(client_socket, uri);
        }
        /* All other requests require valid cookie */
        else if (cookie_header[0] != '\0') {
            /* Profile API endpoint */
            if (strncmp(uri, "/profile?", 9) == 0) {
                result = handle_profile(client_socket, uri);
            }
            /* Other endpoints... */
            else {
                result = (int)serve_file(client_socket, uri);
            }
        }
        else {
            /* No cookie - redirect to login */
            dprintf(client_socket,
                    "HTTP/1.0 302 Found\r\n"
                    "Location: /index.html\r\n\r\n");
            result = 0;
        }
    }

    fclose(client_stream);
    return result;
}
