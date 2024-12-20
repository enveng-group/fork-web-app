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

static int
log_audit(const char *username, const char *action)
{
    FILE *fp;
    time_t now;
    char timestamp[32];
    int ret;

    if (!username || !action) {
        return -1;
    }

    fp = fopen("var/log/audit.log", "a");
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
handle_client(int client_socket, const char *www_root)
{
    char buf[MAX_BUFFER_SIZE];
    char method[16];
    char uri[256];
    char filepath[512];
    char username[256];
    char password[256];
    char *query;
    ssize_t bytes_read;
    struct stat st;
    int file_fd;
    ssize_t read_bytes;
    ssize_t write_result;
    /* Variables used across multiple sections */
    char fullname[256];
    char email[256];
    char project[256];
    char *query_copy;
    char *token;
    char *saveptr;
    char *value;
    /* Shared file handling variables */
    FILE *fp;
    char line[1024];
    const char *rec_path;
    int result;

    /* Initialize strings */
    memset(fullname, 0, sizeof(fullname));
    memset(email, 0, sizeof(email));
    memset(project, 0, sizeof(project));
    rec_path = NULL;

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

    /* Only accept GET method */
    if (strcmp(method, "GET") != 0) {
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
        rec_path = NULL;

        if (strcmp(uri, "/scjv.rec") == 0) {
            rec_path = "var/db/scjv.rec";
        } else if (strcmp(uri, "/w6946.rec") == 0) {
            rec_path = "var/db/w6946.rec";
        } else if (strcmp(uri, "/ms1180.rec") == 0) {
            rec_path = "var/db/ms1180.rec";
        }

        if (rec_path != NULL) {
            /* Open and send .rec file */
            file_fd = open(rec_path, O_RDONLY);
            if (file_fd < 0) {
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
    dprintf(client_socket, "HTTP/1.0 200 OK\r\n\r\n");

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
