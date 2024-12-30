#include "../include/web_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <sys/file.h>
#include <time.h>
#include <string.h>

/* Add log rotation implementation */

int
check_log_size(const char *filename)
{
    struct stat st;

    if (stat(filename, &st) < 0) {
        return -1;
    }

    return st.st_size >= LOG_MAX_SIZE;
}

int
rotate_log(const char *logname)
{
    char oldpath[PATH_MAX];
    char newpath[PATH_MAX];
    int i;

    /* Parameter validation */
    if (!logname) {
        return ERR_PARAM;
    }

    /* Remove oldest log if exists */
    snprintf(oldpath, sizeof(oldpath), "%s/%s.%d%s",
             LOG_DIR, logname, LOG_MAX_FILES, LOG_ARCHIVE_SUFFIX);
    remove(oldpath);

    /* Rotate existing logs */
    for (i = LOG_MAX_FILES - 1; i > 0; i--) {
        snprintf(oldpath, sizeof(oldpath), "%s/%s.%d%s",
                LOG_DIR, logname, i, LOG_ARCHIVE_SUFFIX);
        snprintf(newpath, sizeof(newpath), "%s/%s.%d%s",
                LOG_DIR, logname, i + 1, LOG_ARCHIVE_SUFFIX);
        rename(oldpath, newpath);
    }

    /* Rotate current log to .1 */
    snprintf(oldpath, sizeof(oldpath), "%s/%s%s",
             LOG_DIR, logname, LOG_SUFFIX);
    snprintf(newpath, sizeof(newpath), "%s/%s.1%s",
             LOG_DIR, logname, LOG_ARCHIVE_SUFFIX);

    if (rename(oldpath, newpath) != 0) {
        return ERR_IO;
    }

    return ERR_NONE;
}

/* Modify log_message() to handle rotation */
int
log_message(int severity, const char *username, const char *action, const char *message)
{
    FILE *fp;
    time_t now;
    char timestamp[32];
    char logpath[PATH_MAX];
    int ret;

    /* Validate parameters */
    if (!username || !action || !message) {
        return ERR_PARAM;
    }

    /* Format timestamp */
    now = time(NULL);
    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
                localtime(&now)) == 0) {
        return ERR_INTERNAL;
    }

    /* Check log size and rotate if needed */
    snprintf(logpath, sizeof(logpath), "%s/web%s", LOG_DIR, LOG_SUFFIX);
    if (check_log_size(logpath)) {
        ret = rotate_log("web");
        if (ret != ERR_NONE) {
            return ret;
        }
    }

    /* Open log file in append mode */
    fp = fopen(logpath, "a");
    if (!fp) {
        return ERR_IO;
    }

    /* Write log entry */
    fprintf(fp, "%s|%d|%s|%s|%s\n",
            timestamp, severity, username, action, message);

    fclose(fp);
    return ERR_NONE;
}

int
parse_auth_file(const char *filename, struct user_entry *entries, size_t max_entries)
{
    FILE *fp;
    char line[512];
    size_t count = 0;
    char *token;
    char *saveptr;

    fp = fopen(filename, "r");
    if (!fp) {
        return -1;
    }

    while (fgets(line, sizeof(line), fp) && count < max_entries) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        /* Parse line fields */
        token = strtok_r(line, ":", &saveptr);
        if (!token) continue;
        strncpy(entries[count].username, token, sizeof(entries[count].username) - 1);

        token = strtok_r(NULL, ":", &saveptr);
        if (!token) continue;
        strncpy(entries[count].password, token, sizeof(entries[count].password) - 1);

        /* Parse remaining fields */
        token = strtok_r(NULL, ":", &saveptr);
        if (!token) continue;
        entries[count].uid = atoi(token);

        token = strtok_r(NULL, ":", &saveptr);
        if (!token) continue;
        entries[count].gid = atoi(token);

        token = strtok_r(NULL, ":", &saveptr);
        if (!token) continue;
        strncpy(entries[count].fullname, token, sizeof(entries[count].fullname) - 1);

        token = strtok_r(NULL, ":", &saveptr);
        if (!token) continue;
        strncpy(entries[count].homedir, token, sizeof(entries[count].homedir) - 1);

        token = strtok_r(NULL, ":", &saveptr);
        if (!token) continue;
        strncpy(entries[count].shell, token, sizeof(entries[count].shell) - 1);

        token = strtok_r(NULL, ":", &saveptr);
        entries[count].is_admin = token ? atoi(token) : 0;

        count++;
    }

    fclose(fp);
    return (int)count;
}

/* Similarly modify log_audit() to use rotation */
static int
log_audit(const char *username, const char *action)
{
    FILE *fp;
    time_t now;
    char timestamp[32];
    char logpath[PATH_MAX];
    int ret;

    if (!username || !action) {
        return -1;
    }

    /* Construct log path */
    snprintf(logpath, sizeof(logpath), "%s/audit%s", LOG_DIR, LOG_SUFFIX);

    /* Check log size and rotate if needed */
    if (check_log_size(logpath) == 1) {
        ret = rotate_log("audit");
        if (ret != ERR_NONE) {
            return ret;
        }
    }

    /* Open log file with exclusive lock */
    fp = fopen(logpath, "a");
    if (!fp) {
        return -1;
    }

    /* Get timestamp */
    now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
             localtime(&now));

    /* Lock file for writing */
    ret = flock(fileno(fp), LOCK_EX);
    if (ret == 0) {
        fprintf(fp, "%s|%s|%s\n", timestamp, username, action);
        flock(fileno(fp), LOCK_UN);
    }

    fclose(fp);
    return ret;
}

int
check_auth(const char *username, const char *password)
{
    FILE *fp;
    char line[MAX_BUFFER_SIZE];
    char file_user[256];
    char file_pass[256];
    char *token;

    if (!username || !password) {
        return 0;
    }

    fp = fopen(AUTH_FILE, "r");
    if (!fp) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        /* Parse line - username is first field, password is second */
        token = strtok(line, ":");
        if (!token) {
            continue;
        }
        strncpy(file_user, token, sizeof(file_user) - 1);
        file_user[sizeof(file_user) - 1] = '\0';

        token = strtok(NULL, ":");
        if (!token) {
            continue;
        }
        strncpy(file_pass, token, sizeof(file_pass) - 1);
        file_pass[sizeof(file_pass) - 1] = '\0';

        /* Compare credentials */
        if (strcmp(username, file_user) == 0 &&
            strcmp(password, file_pass) == 0) {
            log_audit(username, ACTION_LOGIN);
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void
parse_query_string(const char *query, char *username, char *password)
{
    char *param;
    char *value;
    char *query_copy;
    char *saveptr1;

    username[0] = '\0';
    password[0] = '\0';

    query_copy = strdup(query);
    if (!query_copy) {
        return;
    }

    param = strtok_r(query_copy, "&", &saveptr1);
    while (param) {
        value = strchr(param, '=');
        if (value) {
            *value = '\0';
            value++;

            if (strcmp(param, "username") == 0) {
                strncpy(username, value, 255);
                username[255] = '\0';
            } else if (strcmp(param, "password") == 0) {
                strncpy(password, value, 255);
                password[255] = '\0';
            }
        }
        param = strtok_r(NULL, "&", &saveptr1);
    }

    free(query_copy);
}

int
handle_create_record(int client_socket, const char *data)
{
    char username[256];
    const char *body;
    const char *username_header;
    size_t i;
    int result;

    /* Response messages */
    const char success_response[] =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: POST\r\n"
        "Access-Control-Allow-Headers: Content-Type, X-Username\r\n\r\n"
        "{\"status\":\"success\",\"message\":\"Record created successfully\"}\r\n";

    const char error_response[] =
        "HTTP/1.0 400 Bad Request\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n\r\n"
        "{\"status\":\"error\",\"message\":\"Invalid record format\"}\r\n";

    const char server_error[] =
        "HTTP/1.0 500 Internal Server Error\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n\r\n"
        "{\"status\":\"error\",\"message\":\"Server error\"}\r\n";

    /* Parameter validation */
    if (data == NULL || client_socket < 0) {
        return ERR_PARAM;
    }

    /* Extract username from header */
    username_header = strstr(data, "X-Username: ");
    if (username_header) {
        username_header += 11; /* Skip "X-Username: " */
        for (i = 0; i < sizeof(username) - 1 && username_header[i] != '\r' && username_header[i] != '\n'; i++) {
            username[i] = username_header[i];
        }
        username[i] = '\0';
    } else {
        username[0] = '\0';
    }

    /* Find start of request body */
    body = strstr(data, "\r\n\r\n");
    if (body == NULL || strlen(body) < 5) {
        write(client_socket, error_response, strlen(error_response));
        return ERR_PARAM;
    }
    body += 4; /* Skip CRLN CRLN */

    /* Basic validation of record format */
    if (strstr(body, "Project_Name") == NULL ||
        strstr(body, "Obligation") == NULL) {
        write(client_socket, error_response, strlen(error_response));
        return ERR_PARAM;
    }

    /* Create the record */
    result = create_record_in_file(body);

    if (result == 0) {
        /* Log success */
        log_message(LOG_INFO, username, "CREATE_RECORD", "Record created successfully");
        log_audit(username, "Record created");
        write(client_socket, success_response, strlen(success_response));
        return 0;
    }

    /* Log failure */
    log_message(LOG_ERROR, username, "CREATE_RECORD", "Failed to create record");

    /* Send error response */
    write(client_socket, server_error, strlen(server_error));
    return -1;
}

int
create_record_in_file(const char *data)
{
    FILE *fp;
    int ret = ERR_NONE;
    char filepath[PATH_MAX];

    /* Input validation */
    if (!data) {
        return ERR_PARAM;
    }

    /* Construct full path */
    if (snprintf(filepath, sizeof(filepath), "%s/scjv.rec", RECORDS_DIR) < 0) {
        return ERR_INTERNAL;
    }

    /* Create directory if it doesn't exist */
    if (mkdir(RECORDS_DIR, 0755) < 0 && errno != EEXIST) {
        return ERR_IO;
    }

    /* Open file with exclusive lock */
    fp = fopen(filepath, "a");
    if (!fp) {
        return ERR_IO;
    }

    /* Get exclusive lock */
    if (flock(fileno(fp), LOCK_EX) == 0) {
        /* Write record */
        if (fprintf(fp, "%s\n", data) < 0) {
            ret = ERR_IO;
        }
        flock(fileno(fp), LOCK_UN);
    } else {
        ret = ERR_IO;
    }

    fclose(fp);
    return ret;
}

int
update_record_in_file(FILE *fp, const char *data)
{
    FILE *temp_fp;
    char line[MAX_BUFFER_SIZE];
    char curr_ob[64];
    char new_ob[64];
    int found;
    const char *start;

    /* Parameter validation */
    if (!fp || !data) {
        return ERR_PARAM;
    }

    memset(curr_ob, 0, sizeof(curr_ob));
    memset(new_ob, 0, sizeof(new_ob));
    found = 0;

    /* Extract new obligation number */
    start = strstr(data, "Obligation_Number:");
    if (!start || sscanf(start, "Obligation_Number: %63s", new_ob) != 1) {
        return ERR_PARAM;
    }

    /* Create temp file */
    temp_fp = fopen("var/records/scjv.rec.tmp", "w");
    if (!temp_fp) {
        return ERR_IO;
    }

    /* Reset position to start of file */
    rewind(fp);

    /* Copy header if present */
    if (fgets(line, sizeof(line), fp) && strstr(line, "%rec: Project")) {
        fputs(line, temp_fp);
        if (fgets(line, sizeof(line), fp) && line[0] == '\n') {
            fputs(line, temp_fp);
        } else {
            rewind(fp);
        }
    } else {
        rewind(fp);
    }

    /* Process file line by line */
    while (fgets(line, sizeof(line), fp)) {
        /* Check if this is start of a record */
        if (strstr(line, "Obligation_Number:")) {
            if (sscanf(line, "Obligation_Number: %63s", curr_ob) == 1) {
                /* If this is our target record */
                if (strcmp(curr_ob, new_ob) == 0) {
                    /* Write new record data without %rec header */
                    if (strstr(data, "%rec: Project")) {
                        const char *record_start = strstr(data, "\n\n");
                        if (record_start) {
                            record_start += 2;
                            fprintf(temp_fp, "%s", record_start);
                        } else {
                            fprintf(temp_fp, "%s", data);
                        }
                    } else {
                        fprintf(temp_fp, "%s", data);
                    }
                    found = 1;

                    /* Skip old record */
                    while (fgets(line, sizeof(line), fp)) {
                        if (line[0] == '\n' || strstr(line, "Project_Name:")) {
                            break;
                        }
                    }
                    if (strstr(line, "Project_Name:")) {
                        fputs(line, temp_fp);
                    }
                    continue;
                }
            }
        }
        fputs(line, temp_fp);
    }

    fclose(temp_fp);

    /* Only replace if record was found and updated */
    if (found) {
        rename("var/records/scjv.rec.tmp", "var/records/scjv.rec");
        return 0;
    }

    remove("var/records/scjv.rec.tmp");
    return ERR_NOTFOUND;
}

int
handle_update_record(int client_socket, const char *data)
{
    FILE *fp;
    const char *body;
    int result;

    /* Parameter validation */
    if (!data) {
        return ERR_PARAM;
    }

    /* Open file for update */
    fp = fopen("var/records/scjv.rec", "r");
    if (!fp) {
        return ERR_IO;
    }

    /* Get exclusive lock */
    if (flock(fileno(fp), LOCK_EX) != 0) {
        fclose(fp);
        return ERR_IO;
    }

    /* Find request body */
    body = strstr(data, "\r\n\r\n");
    if (!body) {
        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        return ERR_PARAM;
    }
    body += 4;

    /* Update record */
    result = update_record_in_file(fp, body);

    /* Cleanup */
    flock(fileno(fp), LOCK_UN);
    fclose(fp);

    /* Send response */
    if (result == 0) {
        dprintf(client_socket,
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n\r\n"
            "{\"status\":\"success\"}\r\n");
    } else {
        dprintf(client_socket,
            "HTTP/1.0 500 Internal Server Error\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n\r\n"
            "{\"status\":\"error\",\"message\":\"Failed to update record\"}\r\n");
    }

    return result;
}


int
handle_client(int client_socket, const char *www_root)
{
    char buf[MAX_BUFFER_SIZE];
    char method[16];
    char uri[256];
    char filepath[512];
    char username[256] = {0};  /* Initialize to zero */
    char password[256] = {0};  /* Initialize to zero */
    char fullname[256] = {0};  /* Initialize to zero */
    char email[256] = {0};     /* Initialize to zero */
    char project[256] = {0};   /* Initialize to zero */
    char line[1024];
    const char *filename;
    char *query;
    char *query_copy;
    char *token;
    char *saveptr;
    char *value;
    ssize_t bytes_read;
    struct stat st;
    int file_fd;
    ssize_t read_bytes;
    ssize_t write_result;
    FILE *fp;
    int result;

    /* Initialize pointers */
    filename = NULL;
    query = NULL;
    query_copy = NULL;
    token = NULL;
    saveptr = NULL;
    value = NULL;
    file_fd = -1;

    /* Read HTTP request */
    bytes_read = read(client_socket, buf, sizeof(buf) - 1);
    if (bytes_read <= 0) {
        fprintf(stderr, "Error: Failed to read request\n");
        return -1;
    }
    buf[bytes_read] = '\0';
    fprintf(stderr, "Received request: %s\n", buf);

    /* Parse HTTP request */
    if (sscanf(buf, "%15s %255s", method, uri) != 2) {
        fprintf(stderr, "Error: Failed to parse request\n");
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }
    fprintf(stderr, "Method: %s, URI: %s\n", method, uri);

    /* Update method check to allow POST */
    if (strcmp(method, "GET") != 0 && strcmp(method, "POST") != 0) {
        dprintf(client_socket, "HTTP/1.0 405 Method Not Allowed\r\n\r\n");
        return -1;
    }

    /* Handle users request - Add this check before file serving */
    if (strcmp(uri, "/users") == 0) {
        fprintf(stderr, "Handling /users request\n");
        result = handle_users_request(client_socket);
        fprintf(stderr, "Users request handled with result: %d\n", result);
        return result;
    }

    /* Handle authentication requests */
    if (strncmp(uri, "/auth?", 6) == 0) {
        query = uri + 6;
        parse_query_string(query, username, password);
        fprintf(stderr, "Auth request - username: %s\n", username);

        if (check_auth(username, password)) {
            fprintf(stderr, "Auth successful\n");
            dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
        } else {
            fprintf(stderr, "Auth failed\n");
            dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n\r\n");
        }
        return 0;
    }

    /* Handle user update requests */
    if (strncmp(uri, "/update?", 8) == 0) {
        query = uri + 8;
        query_copy = strdup(query);

        if (query_copy) {
            token = strtok_r(query_copy, "&", &saveptr);
            while (token) {
                value = strchr(token, '=');
                if (value) {
                    *value++ = '\0';
                    if (strcmp(token, "username") == 0) {
                        strncpy(username, value, sizeof(username) - 1);
                    } else if (strcmp(token, "fullname") == 0) {
                        strncpy(fullname, value, sizeof(fullname) - 1);
                    } else if (strcmp(token, "email") == 0) {
                        strncpy(email, value, sizeof(email) - 1);
                    } else if (strcmp(token, "project") == 0) {
                        strncpy(project, value, sizeof(project) - 1);
                    }
                }
                token = strtok_r(NULL, "&", &saveptr);
            }
            free(query_copy);

            if (username[0] && fullname[0]) {
                return handle_update_user(client_socket, username, fullname,
                                       email, project);
            }
        }
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }

    /* Handle audit log requests */
    if (strcmp(uri, "/audit_log") == 0) {
        fp = fopen("var/log/audit.log", "r");
        if (!fp) {
            dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
            return -1;
        }

        dprintf(client_socket, "HTTP/1.0 200 OK\r\n");
        dprintf(client_socket, "Content-Type: text/plain\r\n\r\n");

        while (fgets(line, sizeof(line), fp)) {
            write(client_socket, line, strlen(line));
        }

        fclose(fp);
        return 0;
    }

    /* Handle .rec file requests */
    if (strstr(uri, ".rec") != NULL) {
        fprintf(stderr, "Received .rec file request for URI: %s\n", uri);

        filename = strrchr(uri, '/');
        if (filename) {
            filename++; /* Skip the slash */
        } else {
            filename = uri; /* No slash found, use full URI */
        }

        /* Construct full path */
        if (snprintf(filepath, sizeof(filepath), "var/records/%s", filename) >= (int)sizeof(filepath)) {
            dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
            return -1;
        }

        fprintf(stderr, "Attempting to serve .rec file: %s\n", filepath);

        /* Open and send .rec file */
        file_fd = open(filepath, O_RDONLY);
        if (file_fd < 0) {
            fprintf(stderr, "Error opening file %s: %s\n", filepath, strerror(errno));
            dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
            return -1;
        }

        /* Send HTTP headers */
        dprintf(client_socket,
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Access-Control-Allow-Origin: *\r\n\r\n");

        /* Send file contents */
        while ((read_bytes = read(file_fd, buf, sizeof(buf))) > 0) {
            write_result = write(client_socket, buf, (size_t)read_bytes);
            if (write_result < 0 || write_result != read_bytes) {
                close(file_fd);
                return -1;
            }
        }

        close(file_fd);
        return 0;
    }

    /* Handle profile view */
    if (strncmp(uri, "/profile.html?", 13) == 0) {
        query = uri + 13;
        parse_query_string(query, username, password);
        if (username[0] != '\0') {
            log_audit(username, ACTION_VIEW_PROFILE);
        }
        /* For profile page, serve the static file regardless of query params */
        if (snprintf(filepath, sizeof(filepath), "%s/profile.html", www_root) >=
            (int)sizeof(filepath)) {
            fprintf(stderr, "Error: Path too long for profile.html\n");
            return -1;
        }
    } else if (strcmp(uri, "/") == 0) {
        if (snprintf(filepath, sizeof(filepath), "%s/index.html", www_root) >= (int)sizeof(filepath)) {
            fprintf(stderr, "Error: Path too long for index.html\n");
            return -1;
        }
    } else {
        if (snprintf(filepath, sizeof(filepath), "%s%s", www_root, uri) >= (int)sizeof(filepath)) {
            fprintf(stderr, "Error: Path too long: %s%s\n", www_root, uri);
            return -1;
        }
    }
    fprintf(stderr, "Attempting to serve file: %s\n", filepath);

    /* Handle project views */
    if (strncmp(uri, "/scjv.html", 9) == 0 ||
        strncmp(uri, "/ms1180.html", 11) == 0 ||
        strncmp(uri, "/w6946.html", 10) == 0) {
        /* Extract username from cookie if present */
        value = strstr(buf, "Cookie:");  /* Reuse value pointer */
        if (value) {
            parse_query_string(value, username, password);
            if (username[0] != '\0') {
                log_audit(username, ACTION_VIEW_PROJECT);
            }
        }
    }

    /* Handle CRUD endpoints */
    if (strcmp(uri, ENDPOINT_CREATE) == 0) {
        return handle_create_record(client_socket, buf);
    }
    else if (strcmp(uri, ENDPOINT_UPDATE) == 0) {
        return handle_update_record(client_socket, buf);
    }

    /* Add handler for get_next_number endpoint */
    if (strncmp(uri, ENDPOINT_NEXT_NUMBER, strlen(ENDPOINT_NEXT_NUMBER)) == 0) {
        return handle_next_number(client_socket);
    }

    /* Check if file exists and is readable */
    if (stat(filepath, &st) < 0 || !S_ISREG(st.st_mode)) {
        fprintf(stderr, "Error: File not found or not regular: %s\n", filepath);
        dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
        return -1;
    }

    file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
        return -1;
    }

    /* Send HTTP response */
    dprintf(client_socket, "HTTP/1.0 200 OK\r\n");
    dprintf(client_socket, "Access-Control-Allow-Origin: *\r\n");
    dprintf(client_socket, "Access-Control-Allow-Methods: GET, POST\r\n");
    dprintf(client_socket, "Access-Control-Allow-Headers: Content-Type, X-Username\r\n");
    dprintf(client_socket, "\r\n");

    /* Send file contents */
    while ((read_bytes = read(file_fd, buf, sizeof(buf))) > 0) {
        write_result = write(client_socket, buf, (size_t)read_bytes);
        if (write_result < 0 || write_result != read_bytes) {
            close(file_fd);
            return -1;
        }
    }

    close(file_fd);
    return 0;
}

int
handle_users_request(int client_socket)
{
    FILE *fp;
    char line[512];
    char *newline;

    /* Send basic headers */
    dprintf(client_socket, "HTTP/1.0 200 OK\r\n"
                          "Content-Type: text/plain\r\n\r\n");

    /* Open and read auth file directly */
    fp = fopen(AUTH_FILE, "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not open %s\n", AUTH_FILE);
        dprintf(client_socket, "ERROR");
        return -1;
    }

    /* Send each valid line */
    while (fgets(line, sizeof(line), fp)) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        /* Remove trailing newline if present */
        newline = strchr(line, '\n');
        if (newline) {
            *newline = '\0';
        }

        /* If line doesn't end with :0 or :1, append :0 */
        if (line[strlen(line) - 2] != ':') {
            dprintf(client_socket, "%s:0\n", line);
        } else {
            dprintf(client_socket, "%s\n", line);
        }
    }

    fclose(fp);
    return 0;
}

/*
 * handle_update_user - Updates user information and logs the changes
 * @client_socket: Socket to send response
 * @username: User's login name
 * @fullname: User's full name
 * @email: User's email address
 * @project: User's project designation
 *
 * Returns 0 on success, -1 on failure
 */
int
handle_update_user(int client_socket, const char *username, const char *fullname,
                  const char *email, const char *project)
{
    FILE *fp;
    char temp_path[PATH_MAX];
    int result;

    /* Mark parameters as used to prevent warnings */
    UNUSED(email);
    UNUSED(project);

    /* Validate parameters */
    if (!username || !fullname || !email || !project) {
        return -1;
    }

    /* Create log entry */
    result = snprintf(temp_path, sizeof(temp_path), "var/log/updates.log");
    if (result < 0 || (size_t)result >= sizeof(temp_path)) {
        return -1;
    }

    fp = fopen(temp_path, "a");
    if (!fp) {
        return -1;
    }

    /* Write update record */
    fprintf(fp, "%s|%s|%s|%s\n", username, fullname, email, project);
    fclose(fp);

    /* Send success response */
    dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
    return 0;
}

int
setup_server(int port)
{
    int server_socket;
    const int enable = 1;
    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
    } addr;

    /* Create socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        return -1;
    }

    /* Set socket options */
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,
                   &enable, sizeof(int)) < 0) {
        close(server_socket);
        return -1;
    }

    /* Initialize server address structure */
    memset(&addr, 0, sizeof(addr));
    addr.sin.sin_family = AF_INET;
    addr.sin.sin_addr.s_addr = INADDR_ANY;
    addr.sin.sin_port = htons((unsigned short)port);

    /* Bind socket */
    if (bind(server_socket, &addr.sa, sizeof(addr.sin)) < 0) {
        close(server_socket);
        return -1;
    }

    /* Listen for connections */
    if (listen(server_socket, SOMAXCONN) < 0) {
        close(server_socket);
        return -1;
    }

    return server_socket;
}

/* filepath: /home/appuser/fork-web-app/src/web_server.c */

int
get_next_obligation_number(void)
{
    FILE *fp;
    char number_str[32];
    char *prefix;
    int number;
    int ret;

    /* Open with exclusive lock for atomic read/write */
    fp = fopen("/home/appuser/fork-web-app/var/records/obligation_number.txt", "r+");
    if (!fp) {
        return -1;
    }

    /* Get exclusive lock */
    if (flock(fileno(fp), LOCK_EX) != 0) {
        fclose(fp);
        return -1;
    }

    /* Read current number */
    if (fgets(number_str, sizeof(number_str), fp) == NULL) {
        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        return -1;
    }

    /* Find prefix */
    prefix = strstr(number_str, "PCEMP-");
    if (!prefix) {
        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        return -1;
    }

    /* Convert to number */
    number = atoi(prefix + 6);
    if (number <= 0) {
        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        return -1;
    }

    /* Write incremented number */
    rewind(fp);
    ret = fprintf(fp, "PCEMP-%d\n", number + 1);
    fflush(fp);

    /* Release lock and close */
    flock(fileno(fp), LOCK_UN);
    fclose(fp);

    return ret < 0 ? -1 : number;
}

int
handle_next_number(int client_socket)
{
    int number;
    char response[256];

    number = get_next_obligation_number();
    if (number < 0) {
        dprintf(client_socket,
            "HTTP/1.0 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Headers: X-Username\r\n"
            "\r\n"
            "Error getting obligation number");
        return -1;
    }

    snprintf(response, sizeof(response),
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Headers: X-Username\r\n"
        "\r\n"
        "PCEMP-%d", number);

    write(client_socket, response, strlen(response));
    return 0;
}
