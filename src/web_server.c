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
#include <sys/file.h>

/* Static function prototypes */
static int handle_record_ops(int client_socket, const char *project, const char *data, int is_read);
static int rotate_log(const char *filepath);
static int check_log_size(const char *filepath);
static const char *get_level_string(int level);

static const char *
get_level_string(int level)
{
    switch(level) {
        case LOG_ERROR: return "ERROR";
        case LOG_WARN:  return "WARN";
        case LOG_INFO:  return "INFO";
        case LOG_DEBUG: return "DEBUG";
        default:        return "UNKNOWN";
    }
}

static int
rotate_log(const char *filepath)
{
    char new_path[MAX_PATH];
    time_t now;
    struct tm *tm_info;

    if (!filepath) {
        return ERR_PARAM;
    }

    now = time(NULL);
    if (now == (time_t)-1) {
        return ERR_IO;
    }

    tm_info = localtime(&now);
    if (!tm_info) {
        return ERR_IO;
    }

    if (snprintf(new_path, sizeof(new_path), "%s.%04d%02d%02d",
                 filepath,
                 tm_info->tm_year + 1900,
                 tm_info->tm_mon + 1,
                 tm_info->tm_mday) < 0) {
        return ERR_IO;
    }

    return rename(filepath, new_path);
}

static int
check_log_size(const char *filepath)
{
    struct stat st;

    if (!filepath) {
        return ERR_PARAM;
    }

    if (stat(filepath, &st) == 0) {
        if (st.st_size >= MAX_LOG_SIZE) {
            return 1;
        }
    }
    return 0;
}

int
log_message(int level, const char *username, const char *action, const char *msg)
{
    FILE *fp;
    time_t now;
    char timestamp[MAX_TIMESTAMP];
    char filepath[MAX_PATH];
    struct tm *tm_info;
    long ret;

    /* Input validation */
    if (!action || !msg) {
        return ERR_PARAM;
    }

    /* Build log path */
    ret = (long)snprintf(filepath, sizeof(filepath), "%s/audit.log", LOG_PATH);
    if (ret < 0L || (size_t)ret >= sizeof(filepath)) {
        return ERR_IO;
    }

    /* Check log size and rotate if needed */
    if (check_log_size(filepath)) {
        if (rotate_log(filepath) != 0) {
            return ERR_IO;
        }
    }

    /* Get timestamp */
    now = time(NULL);
    if (now == (time_t)-1) {
        return ERR_IO;
    }

    tm_info = localtime(&now);
    if (!tm_info) {
        return ERR_IO;
    }

    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info) == 0) {
        return ERR_IO;
    }

    /* Open log file in append mode */
    fp = fopen(filepath, "a");
    if (!fp) {
        return ERR_IO;
    }

    /* Write log entry */
    ret = fprintf(fp, "[%s] [%s] %s: %s - %s\n",
                 timestamp,
                 get_level_string(level),
                 username ? username : "system",
                 action,
                 msg);

    if (ret < 0) {
        fclose(fp);
        return ERR_IO;
    }

    if (fclose(fp) != 0) {
        return ERR_IO;
    }

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

int
check_auth(const char *username, const char *password)
{
    FILE *fp;
    char line[MAX_BUFFER_SIZE];
    char file_user[256];
    char file_pass[256];
    char *token;

    if (!username || !password) {
        log_message(LOG_ERROR, NULL, "AUTH_FAIL", "Invalid parameters");
        return 0;
    }

    fp = fopen(AUTH_FILE, "r");
    if (!fp) {
        log_message(LOG_ERROR, NULL, "AUTH_FAIL", "Cannot open auth file");
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

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

        if (strcmp(username, file_user) == 0 &&
            strcmp(password, file_pass) == 0) {
            log_message(LOG_INFO, username, "AUTH_SUCCESS", NULL);
            fclose(fp);
            return 1;
        }
    }

    log_message(LOG_WARN, username, "AUTH_FAIL", "Invalid credentials");
    fclose(fp);
    return 0;
}

void
parse_query_string(const char *query, char *username, char *password)
{
    char buffer[MAX_BUFFER_SIZE];
    char *param;
    char *value;
    char *saveptr;

    /* Initialize outputs */
    username[0] = '\0';
    password[0] = '\0';

    /* Validate input */
    if (!query || !username || !password) {
        return;
    }

    /* Copy query to local buffer */
    strncpy(buffer, query, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    /* Parse parameters */
    param = strtok_r(buffer, "&", &saveptr);
    while (param) {
        value = strchr(param, '=');
        if (value) {
            *value = '\0';
            value++;

            if (strcmp(param, "username") == 0) {
                strncpy(username, value, 255);
                username[255] = '\0';
            }
            if (strcmp(param, "password") == 0) {
                strncpy(password, value, 255);
                password[255] = '\0';
            }
        }
        param = strtok_r(NULL, "&", &saveptr);
    }
}

int
handle_record_read(const char *filename, int client_socket)
{
    FILE *fp;
    char buf[REC_LINE_MAX];
    size_t bytes_read;

    /* Validate parameters */
    if (!filename || client_socket < 0) {
        return REC_ERR_PARAM;
    }

    /* Open record file */
    fp = fopen(filename, "r");
    if (!fp) {
        return REC_ERR_IO;
    }

    /* Lock file for reading */
    if (flock(fileno(fp), LOCK_SH) == -1) {
        fclose(fp);
        return REC_ERR_IO;
    }

    /* Send HTTP headers */
    dprintf(client_socket, "HTTP/1.0 200 OK\r\n"
                          "Content-Type: text/plain\r\n\r\n");

    /* Read and send file contents */
    while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (write(client_socket, buf, bytes_read) != (ssize_t)bytes_read) {
            flock(fileno(fp), LOCK_UN);
            fclose(fp);
            return REC_ERR_IO;
        }
    }

    /* Release lock and cleanup */
    flock(fileno(fp), LOCK_UN);
    fclose(fp);

    return REC_ERR_NONE;
}

int
handle_record_write(const char *filename, const char *data)
{
    FILE *fp;
    size_t data_len;

    /* Validate parameters */
    if (!filename || !data) {
        return REC_ERR_PARAM;
    }

    /* Check data length */
    data_len = strlen(data);
    if (data_len == 0 || data_len >= REC_LINE_MAX) {
        return REC_ERR_FORMAT;
    }

    /* Open file in append mode */
    fp = fopen(filename, "a");
    if (!fp) {
        return REC_ERR_IO;
    }

    /* Lock file for writing */
    if (flock(fileno(fp), LOCK_EX) == -1) {
        fclose(fp);
        return REC_ERR_IO;
    }

    /* Write data */
    if (fprintf(fp, "%s\n", data) < 0) {
        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        return REC_ERR_IO;
    }

    /* Flush to ensure write */
    if (fflush(fp) != 0) {
        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        return REC_ERR_IO;
    }

    /* Release lock and cleanup */
    flock(fileno(fp), LOCK_UN);
    fclose(fp);

    return REC_ERR_NONE;
}

/* Centralized record file handling */
static int
handle_record_ops(int client_socket, const char *project, const char *data, int is_read)
{
    const char *rec_file;

    /* Map project to rec file */
    if (strcmp(project, "scjv") == 0) {
        rec_file = SCJV_REC;
    } else if (strcmp(project, "ms1180") == 0) {
        rec_file = MS1180_REC;
    } else if (strcmp(project, "w6946") == 0) {
        rec_file = W6946_REC;
    } else {
        return REC_ERR_FORMAT;
    }

    return is_read ?
        handle_record_read(rec_file, client_socket) :
        handle_record_write(rec_file, data);
}

int
handle_create_record(int client_socket, const char *data)
{
    return handle_record_ops(client_socket, "scjv", data, 0);
}

int
handle_client(int client_socket, const char *www_root)
{
    char buf[MAX_BUFFER_SIZE];
    char method[16];
    char uri[256];
    char filepath[512];
    int file_fd;
    ssize_t bytes_read;
    ssize_t bytes_written;

    /* Basic request parsing */
    bytes_read = read(client_socket, buf, sizeof(buf)-1);
    if (bytes_read <= 0) {
        return -1;
    }
    buf[bytes_read] = '\0';

    /* Parse method and URI */
    if (sscanf(buf, "%15s %255s", method, uri) != 2) {
        return -1;
    }

    /* Handle POST create record */
    if (strcmp(method, "POST") == 0 && strncmp(uri, "/create_record", 13) == 0) {
        char *body = strstr(buf, "\r\n\r\n");
        if (body) {
            body += 4;
            handle_create_record(client_socket, body);
            dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
            return 0;
        }
        return -1;
    }

    /* Only accept GET for other requests */
    if (strcmp(method, "GET") != 0) {
        dprintf(client_socket, "HTTP/1.0 405 Not Allowed\r\n\r\n");
        return -1;
    }

    /* Serve .rec files directly */
    if (strstr(uri, ".rec")) {
        file_fd = open(uri+1, O_RDONLY);
        if (file_fd < 0) {
            dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
            return -1;
        }

        /* Send file contents */
        dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
        while ((bytes_read = read(file_fd, buf, sizeof(buf))) > 0) {
            bytes_written = write(client_socket, buf, (size_t)bytes_read);
            if (bytes_written < 0 || bytes_written != bytes_read) {
                close(file_fd);
                return -1;
            }
        }
        if (bytes_read < 0) {
            close(file_fd);
            return -1;
        }
        close(file_fd);
        return 0;
    }

    /* Serve static files */
    snprintf(filepath, sizeof(filepath), "%s%s", www_root, uri);
    file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
        return -1;
    }

    dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
    while ((bytes_read = read(file_fd, buf, sizeof(buf))) > 0) {
        bytes_written = write(client_socket, buf, (size_t)bytes_read);
        if (bytes_written < 0 || bytes_written != bytes_read) {
            close(file_fd);
            return -1;
        }
    }
    if (bytes_read < 0) {
        close(file_fd);
        return -1;
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
        return ERR_IO;
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
    struct sockaddr_in server_addr;
    int server_socket;
    const int enable = 1;

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

    /* Configure server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons((unsigned short)port);

    /* Bind socket */
    if (bind(server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        close(server_socket);
        return -1;
    }

    /* Listen for connections */
    if (listen(server_socket, 10) < 0) {
        close(server_socket);
        return -1;
    }

    return server_socket;
}
