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

/* Add base64 implementation */
static const unsigned char base64_table[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

int
base64_decode(const char *input, char *output, size_t outlen)
{
    size_t i;
    size_t j;
    size_t v;
    size_t inlen;
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;

    /* Input validation */
    if (!input || !output || outlen == 0) {
        return -1;
    }

    /* Get input length */
    inlen = strlen(input);
    if (inlen == 0 || (inlen % 4) != 0) {
        return -1;
    }

    /* Check output buffer size */
    if (outlen < ((inlen / 4) * 3)) {
        return -1;
    }

    /* Process input in blocks of 4 characters */
    for (i = 0, j = 0; i < inlen; i += 4) {
        /* Get values for each base64 character */
        a = base64_table[(unsigned char)input[i]];
        b = base64_table[(unsigned char)input[i + 1]];
        c = base64_table[(unsigned char)input[i + 2]];
        d = base64_table[(unsigned char)input[i + 3]];

        /* Check for invalid characters */
        if (a == 64 || b == 64) {
            return -1;
        }

        /* Decode block */
        v = (a << 18) | (b << 12);

        /* Handle padding */
        if (input[i + 2] != '=') {
            if (c == 64) {
                return -1;
            }
            v |= (c << 6);
            if (input[i + 3] != '=') {
                if (d == 64) {
                    return -1;
                }
                v |= d;
                output[j++] = (char)((v >> 16) & 0xFF);
                output[j++] = (char)((v >> 8) & 0xFF);
                output[j++] = (char)(v & 0xFF);
            } else {
                output[j++] = (char)((v >> 16) & 0xFF);
                output[j++] = (char)((v >> 8) & 0xFF);
                break;
            }
        } else {
            if (input[i + 3] != '=') {
                return -1;
            }
            output[j++] = (char)((v >> 16) & 0xFF);
            break;
        }
    }

    /* Null terminate output */
    output[j] = '\0';
    return (int)j;
}

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

/* Parse query string from URI */
void
parse_query_string(const char *query, char *username, char *password)
{
    char buffer[MAX_BUFFER_SIZE];
    char *token;
    char *saveptr;
    char *key;
    char *value;

    if (query == NULL || username == NULL || password == NULL) {
        return;
    }

    /* Initialize outputs */
    username[0] = '\0';
    password[0] = '\0';

    /* Copy query to work buffer */
    strncpy(buffer, query, MAX_BUFFER_SIZE - 1);
    buffer[MAX_BUFFER_SIZE - 1] = '\0';

    /* Parse parameters */
    token = strtok_r(buffer, "&", &saveptr);
    while (token != NULL) {
        key = token;
        value = strchr(token, '=');
        if (value != NULL) {
            *value = '\0';
            value++;

            if (strcmp(key, "username") == 0) {
                strncpy(username, value, MAX_BUFFER_SIZE - 1);
                username[MAX_BUFFER_SIZE - 1] = '\0';
            } else if (strcmp(key, "password") == 0) {
                strncpy(password, value, MAX_BUFFER_SIZE - 1);
                password[MAX_BUFFER_SIZE - 1] = '\0';
            }
        }
        token = strtok_r(NULL, "&", &saveptr);
    }
}

/* Check authentication credentials */
int
check_auth(const char *username, const char *password)
{
    FILE *fp;
    char line[MAX_BUFFER_SIZE];
    char *stored_user;
    char *stored_pass;
    char *saveptr;
    char *tmp_line;
    int result = -1;

    /* Input validation */
    if (!username || !password) {
        return -1;
    }

    /* Open auth file */
    fp = fopen(AUTH_FILE, "r");
    if (!fp) {
        return -1;
    }

    /* Read line by line */
    while (fgets(line, sizeof(line), fp)) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        /* Remove newline */
        line[strcspn(line, "\n")] = '\0';

        /* Make a copy for strtok_r */
        tmp_line = strdup(line);
        if (!tmp_line) {
            continue;
        }

        /* Parse fields */
        stored_user = strtok_r(tmp_line, ":", &saveptr);
        if (!stored_user) {
            free(tmp_line);
            continue;
        }

        stored_pass = strtok_r(NULL, ":", &saveptr);
        if (!stored_pass) {
            free(tmp_line);
            continue;
        }

        /* Compare username and password */
        if (strcmp(username, stored_user) == 0) {
            /* Accept if password is "x" or matches exactly */
            if (strcmp(stored_pass, "x") == 0 || strcmp(password, stored_pass) == 0) {
                result = 0;
                free(tmp_line);
                break;
            }
        }

        free(tmp_line);
    }

    fclose(fp);
    return result;
}

int
get_auth_header(int client_socket, char *header, size_t size)
{
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_read;
    char *auth_start;
    char *auth_end;

    /* Basic validation */
    if (!header || size == 0 || client_socket < 0) {
        return -1;
    }

    /* Clear header buffer */
    memset(header, 0, size);

    /* Read from socket */
    bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return -1;
    }
    buffer[bytes_read] = '\0';

    /* Find Authorization header */
    auth_start = strstr(buffer, "Authorization: Basic ");
    if (!auth_start) {
        return -1;
    }

    /* Skip "Authorization: Basic " */
    auth_start += 21;

    /* Find end of header */
    auth_end = strstr(auth_start, "\r\n");
    if (!auth_end) {
        return -1;
    }

    /* Copy auth data */
    if ((size_t)(auth_end - auth_start) >= size) {
        return -1;
    }

    strncpy(header, auth_start, (size_t)(auth_end - auth_start));
    header[auth_end - auth_start] = '\0';

    return 0;
}

int
check_admin_auth(const char *auth_header)
{
    char *decoded;
    char *username;
    char *password;
    char *saveptr;
    size_t decoded_len;
    int result;

    if (!auth_header) {
        return 0;
    }

    /* Allocate space for decoded string */
    decoded_len = strlen(auth_header);
    decoded = malloc(decoded_len + 1);
    if (!decoded) {
        return 0;
    }

    /* Base64 decode */
    if (base64_decode(auth_header, decoded, decoded_len) < 0) {
        free(decoded);
        return 0;
    }

    /* Split into username:password */
    username = strtok_r(decoded, ":", &saveptr);
    password = strtok_r(NULL, ":", &saveptr);

    if (!username || !password) {
        free(decoded);
        return 0;
    }

    /* Check credentials */
    result = check_auth(username, password);
    free(decoded);

    return (result == 0);
}

int
handle_record_read(const char *filename, int client_socket)
{
    FILE *fp;
    char buf[REC_LINE_MAX];
    size_t bytes_read;
    struct stat st;
    int result;

    /* Initialize */
    fp = NULL;
    result = REC_ERR_NONE;

    /* Validate parameters */
    if (!filename || client_socket < 0) {
        return REC_ERR_PARAM;
    }

    /* Check file exists and is readable */
    if (stat(filename, &st) != 0) {
        return REC_ERR_IO;
    }

    /* Open record file with proper permissions */
    fp = fopen(filename, "r");
    if (!fp) {
        return REC_ERR_IO;
    }

    /* Set file permissions */
    if (chmod(filename, 0644) != 0) {
        fclose(fp);
        return REC_ERR_IO;
    }

    /* Lock file for reading */
    if (flock(fileno(fp), LOCK_SH) == -1) {
        fclose(fp);
        return REC_ERR_IO;
    }

    /* Send HTTP headers */
    if (dprintf(client_socket, "HTTP/1.0 200 OK\r\n"
                              "Content-Type: text/plain\r\n\r\n") < 0) {
        result = REC_ERR_IO;
        goto cleanup;
    }

    /* Read and send file contents */
    while ((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (write(client_socket, buf, bytes_read) != (ssize_t)bytes_read) {
            result = REC_ERR_IO;
            goto cleanup;
        }
    }

    /* Check for read errors */
    if (ferror(fp)) {
        result = REC_ERR_IO;
    }

cleanup:
    /* Release lock and cleanup */
    flock(fileno(fp), LOCK_UN);
    fclose(fp);

    return result;
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
    char buffer[MAX_BUFFER_SIZE];
    char method[16];
    char uri[MAX_PATH];
    char http_version[16];
    char filepath[MAX_PATH];
    char read_buffer[MAX_BUFFER_SIZE];
    char *auth_start;
    char auth_header[MAX_BUFFER_SIZE];
    char decoded[MAX_BUFFER_SIZE];
    char *username;
    char *password;
    char *saveptr;
    int file_fd;
    ssize_t n;
    ssize_t bytes_read;
    ssize_t bytes_written;
    int ret;
    size_t auth_len;

    /* Initialize */
    file_fd = -1;
    auth_start = NULL;

    /* Input validation */
    if (www_root == NULL) {
        return -1;
    }

    /* Read request */
    n = read(client_socket, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        return -1;
    }
    buffer[n] = '\0';

    /* Parse request line */
    if (sscanf(buffer, "%15s %255s %15s", method, uri, http_version) != 3) {
        dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
        return -1;
    }

    /* Only accept GET method */
    if (strcmp(method, "GET") != 0) {
        dprintf(client_socket, "HTTP/1.0 405 Method Not Allowed\r\n\r\n");
        return -1;
    }

    /* Handle auth request */
    if (strcmp(uri, "/auth") == 0) {
        /* Extract auth header */
        auth_start = strstr(buffer, "Authorization: Basic ");
        if (!auth_start) {
            dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n"
                                 "WWW-Authenticate: Basic realm=\"User Authentication\"\r\n\r\n");
            return -1;
        }

        /* Skip "Authorization: Basic " */
        auth_start += 21;
        auth_len = strcspn(auth_start, "\r\n");
        if (auth_len >= sizeof(auth_header)) {
            dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
            return -1;
        }

        /* Copy auth data */
        memcpy(auth_header, auth_start, auth_len);
        auth_header[auth_len] = '\0';

        /* Decode base64 */
        if (base64_decode(auth_header, decoded, sizeof(decoded)) < 0) {
            dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
            return -1;
        }

        /* Split into username:password */
        username = strtok_r(decoded, ":", &saveptr);
        password = strtok_r(NULL, ":", &saveptr);

        if (!username || !password) {
            dprintf(client_socket, "HTTP/1.0 400 Bad Request\r\n\r\n");
            return -1;
        }

        /* Check credentials */
        ret = check_auth(username, password);
        if (ret == 0) {
            dprintf(client_socket, "HTTP/1.0 200 OK\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "\r\n"
                                 "Authentication successful\n");
            return 0;
        }

        dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n"
                             "WWW-Authenticate: Basic realm=\"User Authentication\"\r\n\r\n");
        return -1;
    }

    /* Handle record file requests */
    if (strstr(uri, ".rec") != NULL) {
        /* Build full path for record file */
        ret = snprintf(filepath, sizeof(filepath), "%s/%s", REC_PATH, uri + 1);
        if (ret < 0 || (size_t)ret >= sizeof(filepath)) {
            dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
            return -1;
        }

        /* Check authorization */
        auth_start = strstr(buffer, "Authorization: Basic ");
        if (!auth_start) {
            dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n"
                                 "WWW-Authenticate: Basic realm=\"Record Access\"\r\n\r\n");
            log_message(LOG_WARN, "system", "ACCESS_DENIED", "No authorization provided");
            return -1;
        }

        /* Open and serve the record file */
        file_fd = open(filepath, O_RDONLY);
        if (file_fd < 0) {
            dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
            return -1;
        }

        dprintf(client_socket, "HTTP/1.0 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Cache-Control: no-cache\r\n"
                             "\r\n");

        while ((bytes_read = read(file_fd, read_buffer, sizeof(read_buffer))) > 0) {
            bytes_written = write(client_socket, read_buffer, (size_t)bytes_read);
            if (bytes_written < 0 || bytes_written != bytes_read) {
                close(file_fd);
                return -1;
            }
        }

        close(file_fd);
        return bytes_read < 0 ? -1 : 0;
    }

    /* Handle regular file requests */
    ret = snprintf(filepath, sizeof(filepath), "%s%s", www_root, uri);
    if (ret < 0 || (size_t)ret >= sizeof(filepath)) {
        dprintf(client_socket, "HTTP/1.0 414 URI Too Long\r\n\r\n");
        return -1;
    }

    /* Security: Prevent path traversal */
    if (strstr(filepath, "..") != NULL) {
        dprintf(client_socket, "HTTP/1.0 403 Forbidden\r\n\r\n");
        return -1;
    }

    file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
        return -1;
    }

    dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");
    while ((bytes_read = read(file_fd, read_buffer, sizeof(read_buffer))) > 0) {
        bytes_written = write(client_socket, read_buffer, (size_t)bytes_read);
        if (bytes_written < 0 || bytes_written != bytes_read) {
            close(file_fd);
            return -1;
        }
    }

    close(file_fd);
    return bytes_read < 0 ? -1 : 0;
}

int
handle_users_request(int client_socket)
{
    FILE *fp;
    char line[512];
    char auth_header[MAX_BUFFER_SIZE];
    char *newline;
    int is_admin;

    /* Initialize variables */
    is_admin = 0;
    fp = NULL;

    /* Check admin privileges first */
    if (get_auth_header(client_socket, auth_header, sizeof(auth_header)) != 0) {
        dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n\r\n");
        return -1;
    }

    is_admin = check_admin_auth(auth_header);
    if (!is_admin) {
        dprintf(client_socket, "HTTP/1.0 403 Forbidden\r\n\r\n");
        return -1;
    }

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
    int server_socket;
    const int enable = 1;
    struct sockaddr server_addr;
    struct sockaddr_in addr_in;

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
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = INADDR_ANY;
    addr_in.sin_port = htons((unsigned short)port);

    /* Copy to sockaddr structure */
    memset(&server_addr, 0, sizeof(server_addr));
    memcpy(&server_addr, &addr_in, sizeof(addr_in));

    /* Bind socket */
    if (bind(server_socket, &server_addr, sizeof(addr_in)) < 0) {
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
