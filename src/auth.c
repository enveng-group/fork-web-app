/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include "../include/auth.h"
#include "../include/logger.h"
#include "../include/data_structures.h"
#include "../include/database.h"
#include "../include/utils.h"   /* Add this for generateUUID */

static struct UserSession *sessions = NULL;
static size_t session_count = 0;
static char passwd_file_path[PATH_MAX];
static pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;

static int initializeSessions(void)
{
    pthread_mutex_lock(&session_mutex);

    if (!sessions) {
        sessions = (struct UserSession *)malloc(MAX_SESSIONS * sizeof(struct UserSession));
        if (!sessions) {
            pthread_mutex_unlock(&session_mutex);
            return -1;
        }
        memset(sessions, 0, MAX_SESSIONS * sizeof(struct UserSession));
        session_count = 0;
    }

    pthread_mutex_unlock(&session_mutex);
    return 0;
}

int authInit(const char *passwd_file)
{
    struct stat st;
    FILE *fp;

    if (!passwd_file) {
        logError("No password file specified");
        return -1;
    }

    /* Store passwd file path */
    strncpy(passwd_file_path, passwd_file, PATH_MAX - 1);
    passwd_file_path[PATH_MAX - 1] = '\0';

    /* Check file permissions */
    if (stat(passwd_file, &st) != 0) {
        logError("Cannot stat password file");
        return -1;
    }

    if ((st.st_mode & 077) != 0) {
        logError("Password file has incorrect permissions");
        return -1;
    }

    fp = fopen(passwd_file, "r");
    if (!fp) {
        logError("Cannot open password file");
        return -1;
    }

    fclose(fp);
    return 0;
}

int authLoadPasswdEntry(const char *username, struct PasswdEntry *entry)
{
    FILE *fp;
    char line[PASSWD_LINE_MAX];
    char *token;

    if (!username || !entry) {
        logError("Invalid parameters in authLoadPasswdEntry");
        return -1;
    }

    if (strlen(passwd_file_path) == 0) {
        logError("Password file path not initialized");
        return -1;
    }

    fp = fopen(passwd_file_path, "r");
    if (!fp) {
        logError("Cannot open password file: %s", passwd_file_path);
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#') continue;

        token = strtok(line, PASSWD_DELIM);
        if (token && strcmp(token, username) == 0) {
            strncpy(entry->username, token, MAX_USERNAME - 1);

            token = strtok(NULL, PASSWD_DELIM);
            if (token) strncpy(entry->password_hash, token, 127);

            token = strtok(NULL, PASSWD_DELIM);
            if (token) entry->uid = (uid_t)atoi(token);

            token = strtok(NULL, PASSWD_DELIM);
            if (token) entry->gid = (gid_t)atoi(token);

            token = strtok(NULL, PASSWD_DELIM);
            if (token) strncpy(entry->full_name, token, 255);

            token = strtok(NULL, PASSWD_DELIM);
            if (token) strncpy(entry->home_dir, token, PATH_MAX - 1);

            token = strtok(NULL, PASSWD_DELIM);
            if (token) {
                char *newline = strchr(token, '\n');
                if (newline) *newline = '\0';
                strncpy(entry->shell, token, 127);
            }

            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -1;
}

int authValidateUser(const char *username, const char *password)
{
    struct PasswdEntry passwd_entry;
    int result;

    /* Input validation */
    if (!username || !password) {
        logError("Invalid username or password pointer");
        return -1;
    }

    if (strlen(username) >= MAX_USERNAME) {
        logError("Username too long");
        return -1;
    }

    /* Load user entry */
    result = authLoadPasswdEntry(username, &passwd_entry);
    if (result != 0) {
        logError("Failed to load password entry for user %s", username);
        return -1;
    }

    /* Verify password */
    result = authVerifyPassword(password, passwd_entry.password_hash);
    if (result != 0) {
        logError("Password verification failed for user %s", username);
        return -1;
    }

    /* Clear sensitive data */
    memset(&passwd_entry, 0, sizeof(struct PasswdEntry));

    return 0;
}

int authGetUserProfile(const char *username, struct UserProfile *profile)
{
    struct PasswdEntry entry;
    int result;

    if (!username || !profile) {
        return -1;
    }

    result = authLoadPasswdEntry(username, &entry);
    if (result != 0) {
        return result;
    }

    /* Initialize profile with passwd data */
    memset(profile, 0, sizeof(struct UserProfile));
    strncpy(profile->username, entry.username, MAX_USERNAME - 1);
    strncpy(profile->responsibility, entry.full_name, sizeof(profile->responsibility) - 1);
    profile->last_login = time(NULL);

    return 0;
}

int authUpdateUserProfile(const char *username, const struct UserProfile *profile)
{
    struct UserProfile current_profile;
    int result;

    /* Input validation */
    if (!username || !profile)
    {
        logError("Invalid username or profile pointer");
        return -1;
    }

    if (strlen(username) >= MAX_USERNAME)
    {
        logError("Username too long");
        return -1;
    }

    /* Load current profile */
    result = authGetUserProfile(username, &current_profile);
    if (result != 0)
    {
        logError("Failed to load existing profile for user %s", username);
        return -1;
    }

    /* Update profile fields */
    strncpy(current_profile.email, profile->email, 255);
    current_profile.email[255] = '\0';

    strncpy(current_profile.title, profile->title, 127);
    current_profile.title[127] = '\0';

    strncpy(current_profile.phone, profile->phone, 31);
    current_profile.phone[31] = '\0';

    strncpy(current_profile.pronouns, profile->pronouns, 31);
    current_profile.pronouns[31] = '\0';

    strncpy(current_profile.responsibility, profile->responsibility, 255);
    current_profile.responsibility[255] = '\0';

    /* Copy group information */
    if (profile->group_count > MAX_GROUPS)
    {
        logError("Too many groups specified");
        return -1;
    }

    current_profile.group_count = profile->group_count;
    memcpy(current_profile.groups, profile->groups,
           sizeof(int) * profile->group_count);

    /* Update permissions */
    current_profile.permissions = profile->permissions;
    current_profile.last_login = time(NULL);

    /* Save updated profile */
    result = authSaveUserProfile(&current_profile);
    if (result != 0)
    {
        logError("Failed to save updated profile for user %s", username);
        return -1;
    }

    return 0;
}

int authSaveUserProfile(const struct UserProfile *profile)
{
    struct PasswdEntry entry;
    int result;

    if (!profile) {
        return -1;
    }

    /* Load existing entry */
    result = authLoadPasswdEntry(profile->username, &entry);
    if (result != 0) {
        return result;
    }

    /* Update relevant fields */
    strncpy(entry.full_name, profile->responsibility, sizeof(entry.full_name) - 1);

    return authSavePasswdEntry(&entry);
}

/* First setupAuthUsers function */
int setupAuthUsers(struct DBHandle *db)
{
    struct ProjectRecord *records;
    size_t count;
    FILE *passwd_file;
    char *responsibilities[1000];
    int unique_count;
    size_t i;
    int j;
    char username[32];
    char *p;
    int found;

    unique_count = 0;

    /* Get all projects */
    if (dbListProjects(db, &records, &count) != 0) {
        return -1;
    }

    /* Extract unique responsibilities */
    for (i = 0; i < count; i++) {
        found = 0;
        for (j = 0; j < unique_count; j++) {
            if (strcmp(responsibilities[j], records[i].responsibility) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            responsibilities[unique_count++] = strdup(records[i].responsibility);
        }
    }

    /* Create passwd file entries */
    passwd_file = fopen("../etc/auth/passwd", "w");
    if (!passwd_file) {
        return -1;
    }

    fprintf(passwd_file, "# username:password_hash:uid:gid:full_name:home_dir:shell\n");
    fprintf(passwd_file, "admin:$6$xyz123...:1000:1000:Admin User:/home/admin:/bin/false\n");

    /* Add entries for each responsibility */
    for (i = 0; i < (size_t)unique_count; i++) {
        /* Convert responsibility to username format */
        snprintf(username, sizeof(username), "%s", responsibilities[i]);

        /* Replace spaces with underscores */
        p = username;
        while (*p) {
            if (*p == ' ') *p = '_';
            p++;
        }

        fprintf(passwd_file, "%s:$6$abc456...:100%d:100%d:%s:/home/%s:/bin/false\n",
                username, (int)i+1, (int)i+1, responsibilities[i], username);
    }

    fclose(passwd_file);
    return 0;
}

/* Second setupAuthUsers function - renamed to avoid conflict */
int setupAuthUsersFromList(const char **users, size_t count)
{
    size_t i;
    size_t j;
    int unique_count;
    char *username;
    char **unique_users;
    char *p;
    int is_duplicate;

    /* Allocate memory for unique users array */
    unique_users = (char **)malloc(count * sizeof(char *));
    if (!unique_users) {
        logError("Memory allocation failed for unique users");
        return -1;
    }

    unique_count = 0;

    /* Find unique users */
    for (i = 0; i < count; i++) {
        is_duplicate = 0;
        for (j = 0; j < (size_t)unique_count; j++) {
            if (strcmp(users[i], unique_users[j]) == 0) {
                is_duplicate = 1;
                break;
            }
        }

        if (!is_duplicate) {
            username = strdup(users[i]);
            if (!username) {
                logError("Memory allocation failed for username");
                for (j = 0; j < (size_t)unique_count; j++) {
                    free(unique_users[j]);
                }
                free(unique_users);
                return -1;
            }
            unique_users[unique_count++] = username;
        }
    }

    /* Validate usernames */
    for (i = 0; i < (size_t)unique_count; i++) {
        username = unique_users[i];
        if (strlen(username) > DS_MAX_USERNAME_LEN) {
            logError("Username too long: %s", username);
            p = username;
            while (*p) {
                if (!isalnum(*p) && *p != '_' && *p != '-') {
                    logError("Invalid character in username: %s", username);
                    break;
                }
                p++;
            }
        }
    }

    /* Cleanup */
    for (i = 0; i < (size_t)unique_count; i++) {
        free(unique_users[i]);
    }
    free(unique_users);

    return 0;
}

int authCreateSession(const char *username, struct UserSession *session)
{
    time_t now;
    char uuid[37];  /* Match the size from generateUUID */
    int i;

    if (!username || !session) {
        return -1;
    }

    if (!sessions && initializeSessions() != 0) {
        return -1;
    }

    pthread_mutex_lock(&session_mutex);

    /* Check if we have room for new session */
    if (session_count >= MAX_SESSIONS) {
        pthread_mutex_unlock(&session_mutex);
        return -1;
    }

    /* Generate session ID */
    if (generateUUID() == NULL) {
        pthread_mutex_unlock(&session_mutex);
        return -1;
    }
    strncpy(uuid, generateUUID(), sizeof(uuid) - 1);
    now = time(NULL);

    /* Find empty slot */
    for (i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].creation_time == 0) {
            strncpy(sessions[i].session_id, uuid, MAX_SESSION_ID - 1);
            strncpy(sessions[i].username, username, MAX_USERNAME - 1);
            sessions[i].creation_time = now;
            sessions[i].last_access = now;
            sessions[i].permissions = PERM_READ; /* Default permission */

            /* Copy to output parameter */
            memcpy(session, &sessions[i], sizeof(struct UserSession));

            session_count++;
            pthread_mutex_unlock(&session_mutex);
            return 0;
        }
    }

    pthread_mutex_unlock(&session_mutex);
    return -1;
}

int authValidateSession(const char *session_id)
{
    time_t now;
    int i;

    if (!session_id || !sessions) {
        return -1;
    }

    now = time(NULL);
    pthread_mutex_lock(&session_mutex);

    for (i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].creation_time != 0 &&
            strcmp(sessions[i].session_id, session_id) == 0) {

            /* Check if session has expired */
            if (now - sessions[i].last_access > SESSION_TIMEOUT) {
                /* Remove expired session */
                memset(&sessions[i], 0, sizeof(struct UserSession));
                if (session_count > 0) {
                    session_count--;
                }
                pthread_mutex_unlock(&session_mutex);
                return -1;
            }

            /* Update last access time */
            sessions[i].last_access = now;
            pthread_mutex_unlock(&session_mutex);
            return 0;
        }
    }

    pthread_mutex_unlock(&session_mutex);
    return -1;
}

void authCleanup(void)
{
    pthread_mutex_lock(&session_mutex);

    if (sessions) {
        free(sessions);
        sessions = NULL;
    }
    session_count = 0;

    pthread_mutex_unlock(&session_mutex);
    pthread_mutex_destroy(&session_mutex);
}

int authVerifyPassword(const char *password, const char *hash)
{
    char *crypted;
    char salt[13];
    int i;

    if (!password || !hash) {
        logError("Invalid password or hash pointer");
        return -1;
    }

    /* Extract salt from hash (first 12 chars) */
    for (i = 0; i < 12 && hash[i] != '\0'; i++) {
        salt[i] = hash[i];
    }
    salt[i] = '\0';

    /* Use crypt() to hash the password with the same salt */
    crypted = crypt(password, salt);
    if (!crypted) {
        logError("Password encryption failed");
        return -1;
    }

    /* Compare the hashed password with stored hash */
    if (strcmp(crypted, hash) != 0) {
        return -1;
    }

    return 0;
}

int authSavePasswdEntry(const struct PasswdEntry *entry)
{
    FILE *fp;
    FILE *tmp;
    char line[PASSWD_LINE_MAX];
    char tmp_path[PATH_MAX];
    char *username;
    int found = 0;

    if (!entry) {
        logError("Invalid password entry pointer");
        return -1;
    }

    /* Create temporary file path */
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", passwd_file_path);

    /* Open existing passwd file for reading */
    fp = fopen(passwd_file_path, "r");
    if (!fp) {
        logError("Cannot open password file for reading");
        return -1;
    }

    /* Open temporary file for writing */
    tmp = fopen(tmp_path, "w");
    if (!tmp) {
        logError("Cannot open temporary password file for writing");
        fclose(fp);
        return -1;
    }

    /* Copy contents while replacing matching entry */
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#') {
            fprintf(tmp, "%s", line);
            continue;
        }

        username = strtok(line, ":");
        if (username && strcmp(username, entry->username) == 0) {
            /* Write new entry */
            fprintf(tmp, "%s:%s:%d:%d:%s:%s:%s\n",
                   entry->username,
                   entry->password_hash,
                   entry->uid,
                   entry->gid,
                   entry->full_name,
                   entry->home_dir,
                   entry->shell);
            found = 1;
        } else {
            fprintf(tmp, "%s", line);
        }
    }

    /* Add new entry if not found */
    if (!found) {
        fprintf(tmp, "%s:%s:%d:%d:%s:%s:%s\n",
               entry->username,
               entry->password_hash,
               entry->uid,
               entry->gid,
               entry->full_name,
               entry->home_dir,
               entry->shell);
    }

    /* Close files */
    fclose(fp);
    fclose(tmp);

    /* Replace original file with temporary file */
    if (rename(tmp_path, passwd_file_path) != 0) {
        logError("Failed to update password file");
        unlink(tmp_path);
        return -1;
    }

    /* Set correct permissions */
    if (chmod(passwd_file_path, 0600) != 0) {
        logError("Failed to set password file permissions");
        return -1;
    }

    return 0;
}
