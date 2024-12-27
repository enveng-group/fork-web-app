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
static const char *get_mime_type(const char *filename);
static int serve_file(int client_socket, const char *filepath);

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

/* base64_decode function with proper validation */

int
base64_decode(const char *input, char *output, size_t outlen)
{
    size_t i, j;
    size_t v;
    size_t inlen;
    unsigned char a, b, c, d;

    /* Validate inputs */
    if (!input || !output || outlen == 0) {
        return -1;
    }

    /* Get input length and validate */
    inlen = strlen(input);
    if (inlen == 0 || (inlen % 4) != 0 || inlen >= outlen * 4/3) {
        return -1;
    }

    /* Check output buffer size */
    if (outlen < ((inlen / 4) * 3)) {
        return -1;
    }

    /* Process input in blocks of 4 characters */
    for (i = 0, j = 0; i < inlen; i += 4) {
        /* Check for invalid base64 table entries */
        if (base64_table[(unsigned char)input[i]] == 64 ||
            base64_table[(unsigned char)input[i+1]] == 64 ||
            base64_table[(unsigned char)input[i+2]] == 64 ||
            base64_table[(unsigned char)input[i+3]] == 64) {
            return -1;
        }

        a = base64_table[(unsigned char)input[i]];
        b = base64_table[(unsigned char)input[i+1]];
        c = base64_table[(unsigned char)input[i+2]];
        d = base64_table[(unsigned char)input[i+3]];

        v = (a << 18) | (b << 12) | (c << 6) | d;

        output[j++] = (v >> 16) & 0xFF;
        if (input[i+2] != '=') {
            output[j++] = (v >> 8) & 0xFF;
        }
        if (input[i+3] != '=') {
            output[j++] = v & 0xFF;
        }
    }

    output[j] = '\0';
    return (int)j;
}


/* Modify static functions to be used */
static const char *
get_level_string(int level)
{
    switch (level) {
        case LOG_ERROR:
            return "ERROR";
        case LOG_WARN:
            return "WARN";
        case LOG_INFO:
            return "INFO";
        case LOG_DEBUG:
            return "DEBUG";
        default:
            return NULL;
    }
}

static int
rotate_log(const char *filepath)
{
    char old_path[MAX_PATH];
    char new_path[MAX_PATH];
    int i;

    /* Rotate existing backup files */
    for (i = MAX_BACKUP_COUNT - 1; i > 0; i--) {
        snprintf(old_path, sizeof(old_path), "%s.%d", filepath, i);
        snprintf(new_path, sizeof(new_path), "%s.%d", filepath, i + 1);
        rename(old_path, new_path);
    }

    /* Rotate current file */
    snprintf(new_path, sizeof(new_path), "%s.1", filepath);
    if (rename(filepath, new_path) != 0) {
        return ERR_IO;
    }

    return ERR_NONE;
}

static int
check_log_size(const char *filepath)
{
    struct stat st;

    if (!filepath) {
        return ERR_PARAM;
    }

    if (stat(filepath, &st) != 0) {
        return ERR_NONE; /* File doesn't exist yet */
    }

    return (st.st_size >= MAX_LOG_SIZE) ? 1 : 0;
}

int
log_message(int level, const char *username, const char *action, const char *msg)
{
    FILE *fp;
    time_t now;
    char timestamp[MAX_TIMESTAMP];
    struct tm *tm_info;
    int ret;
    const char *level_str;

    /* Input validation */
    if (!username || !action || !msg) {
        return ERR_PARAM;
    }

    /* Get level string */
    level_str = get_level_string(level);
    if (!level_str) {
        return ERR_PARAM;
    }

    /* Check log size and rotate if needed */
    if (check_log_size(AUDIT_LOG)) {
        if (rotate_log(AUDIT_LOG) != ERR_NONE) {
            return ERR_IO;
        }
    }

    /* Get current time */
    now = time(NULL);
    if (now == (time_t)-1) {
        return ERR_IO;
    }

    tm_info = localtime(&now);
    if (!tm_info) {
        return ERR_IO;
    }

    if (strftime(timestamp, sizeof(timestamp), LOG_TIME_FMT, tm_info) == 0) {
        return ERR_IO;
    }

    /* Open and lock log file */
    fp = fopen(AUDIT_LOG, "a");
    if (!fp) {
        return ERR_IO;
    }

    if (flock(fileno(fp), LOCK_EX) == -1) {
        fclose(fp);
        return ERR_IO;
    }

    /* Write log entry */
    ret = fprintf(fp, LOG_ENTRY_FMT, timestamp, level_str,
                 username, action, msg);

    flock(fileno(fp), LOCK_UN);
    fclose(fp);

    return (ret < 0) ? ERR_IO : ERR_NONE;
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
get_auth_header(int client_socket, char *auth_header, size_t max_len)
{
    char buffer[MAX_BUFFER_SIZE];
    char *line;
    char *saveptr = NULL;
    ssize_t bytes_read;

    /* Initialize output */
    if (auth_header == NULL || max_len == 0) {
        return -1;
    }
    auth_header[0] = '\0';

    /* Read request headers */
    bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return -1;
    }
    buffer[bytes_read] = '\0';

    /* Parse headers line by line */
    line = strtok_r(buffer, "\r\n", &saveptr);
    while (line != NULL) {
        if (strncmp(line, "Authorization: Basic ", 20) == 0) {
            strncpy(auth_header, line + 20, max_len - 1);
            auth_header[max_len - 1] = '\0';
            return 0;
        }
        line = strtok_r(NULL, "\r\n", &saveptr);
    }

    return -1;
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

/* CRUD Operations */
int
handle_record_write(const char *filename, const char *data)
{
    FILE *fp;
    char filepath[PATH_MAX];
    int ret;

    ret = snprintf(filepath, sizeof(filepath), "%s/%s", REC_PATH, filename);
    if (ret < 0 || (size_t)ret >= sizeof(filepath)) {
        return REC_ERR_PARAM;
    }

    fp = fopen(filepath, "a");
    if (!fp) {
        return REC_ERR_IO;
    }

    if (flock(fileno(fp), LOCK_EX) == -1) {
        fclose(fp);
        return REC_ERR_IO;
    }

    fprintf(fp, "%s\n", data);

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

static const char *
get_mime_type(const char *filename)
{
    const char *ext;

    if (filename == NULL)
        return "text/plain; charset=ISO-8859-1";

    ext = strrchr(filename, '.');
    if (ext == NULL)
        return "text/plain; charset=ISO-8859-1";

    ext++;

    /* Match file extensions */
    if (strcmp(ext, "html") == 0) {
        return "text/html; charset=ISO-8859-1";
    } else if (strcmp(ext, "css") == 0) {
        return "text/css";
    } else if (strcmp(ext, "js") == 0) {
        return "text/javascript";
    } else if (strcmp(ext, "jpg") == 0 || strcmp(ext, "jpeg") == 0) {
        return "image/jpeg";
    } else if (strcmp(ext, "png") == 0) {
        return "image/png";
    } else if (strcmp(ext, "gif") == 0) {
        return "image/gif";
    } else if (strcmp(ext, "ico") == 0) {
        return "image/x-icon";
    } else if (strcmp(ext, "webp") == 0) {
        return "image/webp";
    } else if (strcmp(ext, "svg") == 0) {
        return "image/svg+xml";
    } else if (strcmp(ext, "rec") == 0) {
        return "text/plain; charset=ISO-8859-1";
    } else if (strcmp(ext, "desc") == 0) {
        return "text/plain; charset=ISO-8859-1";
    } else if (strcmp(ext, "passwd") == 0) {
        return "text/plain; charset=ISO-8859-1";
    } else if (strcmp(ext, "log") == 0) {
        return "text/plain; charset=ISO-8859-1";
    }

    return "text/plain; charset=ISO-8859-1";
}

/* Static file serving */
static int
serve_file(int client_socket, const char *filepath)
{
    char buffer[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE];
    FILE *file;
    size_t bytes;
    const char *mime_type;

    file = fopen(filepath, "rb");
    if (!file) {
        dprintf(client_socket, "HTTP/1.0 404 Not Found\r\n\r\n");
        return -1;
    }

    mime_type = get_mime_type(filepath);
    snprintf(response, sizeof(response),
             "HTTP/1.0 200 OK\r\n"
             "Content-Type: %s\r\n"
             "\r\n", mime_type);

    write(client_socket, response, strlen(response));

    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(client_socket, buffer, bytes);
    }

    fclose(file);
    return 0;
}

/* Add helper to check filename extension */
static int
has_extension(const char *filename, const char *ext)
{
    const char *file_ext = strrchr(filename, '.');
    return file_ext && strcmp(file_ext + 1, ext) == 0;
}

/* Add record file authentication check */
static int
check_record_access(const char *path, const char *auth_header)
{
    /* Require authentication for all .rec files */
    if (has_extension(path, "rec")) {
        if (!auth_header) {
            return 0;
        }
        return check_admin_auth(auth_header);
    }
    return 1;
}

int
handle_client(int client_socket, const char *www_root)
{
    /* Variable declarations at start of function */
    char buffer[MAX_BUFFER_SIZE];
    char method[16];
    char uri[MAX_PATH];
    char http_version[16];
    char filepath[MAX_PATH];
    char clean_path[MAX_PATH];
    char auth_header[MAX_BUFFER_SIZE];
    char *auth_start;
    size_t auth_len;
    size_t i;
    size_t j;
    ssize_t n;
    int ret;

    /* Input validation */
    if (www_root == NULL) {
        return -1;
    }

    /* Initialize buffers */
    memset(buffer, 0, sizeof(buffer));
    memset(method, 0, sizeof(method));
    memset(uri, 0, sizeof(uri));
    memset(http_version, 0, sizeof(http_version));
    memset(filepath, 0, sizeof(filepath));
    memset(clean_path, 0, sizeof(clean_path));
    memset(auth_header, 0, sizeof(auth_header));
    auth_start = NULL;

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

    /* Handle authentication */
    if (has_extension(uri, "rec") || has_extension(uri, "log")) {
        if (get_auth_header(client_socket, auth_header, sizeof(auth_header)) != 0 ||
            !check_admin_auth(auth_header)) {
            dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n"
                                "WWW-Authenticate: Basic realm=\"Admin Access\"\r\n\r\n");
            return -1;
        }
    }

    /* Handle special cases */
    if (strcmp(uri, "/users") == 0) {
        return handle_users_request(client_socket);
    }

    if (strncmp(uri, "/record/", 8) == 0) {
        if (strcmp(method, "GET") == 0) {
            return handle_record_read(uri + 8, client_socket);
        } else if (strcmp(method, "POST") == 0) {
            char *data;
            data = strstr(buffer, "\r\n\r\n");
            if (!data) {
                return -1;
            }
            data += 4;
            return handle_record_write(uri + 8, data);
        }
    }

    /* Clean the path to prevent directory traversal */
    for (i = 0, j = 0; uri[i] != '\0' && j < sizeof(clean_path) - 1; i++) {
        if (uri[i] == '.' && uri[i+1] == '.') {
            continue;
        }
        clean_path[j++] = uri[i];
    }
    clean_path[j] = '\0';

    /* Get auth header if present */
    auth_start = strstr(buffer, "Authorization: Basic ");
    if (auth_start != NULL) {
        auth_start += 21; /* Length of "Authorization: Basic " */
        auth_len = strcspn(auth_start, "\r\n");
        if (auth_len < sizeof(auth_header)) {
            memcpy(auth_header, auth_start, auth_len);
            auth_header[auth_len] = '\0';
        }
    }

    /* Handle .rec file requests */
    if (has_extension(clean_path, "rec")) {
        /* Check authentication */
        if (!check_record_access(clean_path, auth_start ? auth_header : NULL)) {
            dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n"
                                 "WWW-Authenticate: Basic realm=\"Record Access\"\r\n\r\n");
            return -1;
        }

        /* Construct full filepath */
        ret = snprintf(filepath, sizeof(filepath), "%s%s", www_root, clean_path);
        if (ret < 0 || (size_t)ret >= sizeof(filepath)) {
            dprintf(client_socket, "HTTP/1.0 414 URI Too Long\r\n\r\n");
            return -1;
        }

        /* For GET requests, serve the record file */
        if (strcmp(method, "GET") == 0) {
            return handle_record_read(filepath, client_socket);
        }

        dprintf(client_socket, "HTTP/1.0 405 Method Not Allowed\r\n\r\n");
        return -1;
    }

    /* Handle root path */
    if (strcmp(clean_path, "/") == 0) {
        ret = snprintf(filepath, sizeof(filepath), "%s/index.html", www_root);
        if (ret < 0 || (size_t)ret >= sizeof(filepath)) {
            dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
            return -1;
        }
        return serve_file(client_socket, filepath);
    }

    /* Serve static files */
    ret = snprintf(filepath, sizeof(filepath), "%s%s",
                  www_root, strcmp(uri, "/") == 0 ? "/index.html" : uri);
    if (ret < 0 || (size_t)ret >= sizeof(filepath)) {
        dprintf(client_socket, "HTTP/1.0 414 URI Too Long\r\n\r\n");
        return -1;
    }

    return serve_file(client_socket, filepath);
}

int
handle_users_request(int client_socket)
{
    char auth_header[MAX_BUFFER_SIZE];
    struct user_entry entries[100];
    int count;
    int i;
    ssize_t bytes_written;
    char response[MAX_BUFFER_SIZE];
    int response_len;

    /* Get auth header */
    if (get_auth_header(client_socket, auth_header, sizeof(auth_header)) != 0) {
        dprintf(client_socket, "HTTP/1.0 401 Unauthorized\r\n"
                             "WWW-Authenticate: Basic realm=\"Admin Access\"\r\n\r\n");
        return -1;
    }

    /* Verify admin privileges */
    if (!check_admin_auth(auth_header)) {
        dprintf(client_socket, "HTTP/1.0 403 Forbidden\r\n\r\n");
        return -1;
    }

    /* Parse auth file */
    count = parse_auth_file(AUTH_FILE, entries, sizeof(entries)/sizeof(entries[0]));
    if (count < 0) {
        dprintf(client_socket, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
        return -1;
    }

    /* Send success response header */
    response_len = snprintf(response, sizeof(response),
                          "HTTP/1.0 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n");

    if (response_len < 0 || (size_t)response_len >= sizeof(response)) {
        return -1;
    }

    bytes_written = write(client_socket, response, (size_t)response_len);
    if (bytes_written < 0 || bytes_written != response_len) {
        return -1;
    }

    /* Output each user */
    for (i = 0; i < count; i++) {
        response_len = snprintf(response, sizeof(response),
                              "%s:%s:%d:%d:%s:%s:%s:%d\n",
                              entries[i].username,
                              "*****",  /* Don't send actual passwords */
                              entries[i].uid,
                              entries[i].gid,
                              entries[i].fullname,
                              entries[i].homedir,
                              entries[i].shell,
                              entries[i].is_admin);

        if (response_len < 0 || (size_t)response_len >= sizeof(response)) {
            return -1;
        }

        bytes_written = write(client_socket, response, (size_t)response_len);
        if (bytes_written < 0 || bytes_written != response_len) {
            return -1;
        }
    }

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
