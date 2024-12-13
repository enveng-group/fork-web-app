/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef AUTH_H
#define AUTH_H

#include <pwd.h>
#include <limits.h>  /* Add this for PATH_MAX */

#define MAX_SESSION_ID 64
#define MAX_USERNAME 32
#define MAX_GROUPS 16
#define MAX_SESSIONS 1000      /* Add this */
#define SESSION_TIMEOUT 3600   /* Add this: 1 hour timeout */
#define PASSWD_LINE_MAX 512
#define PASSWD_DELIM ":"

enum UserPermission {
    PERM_NONE = 0,
    PERM_READ = 1,
    PERM_WRITE = 2,
    PERM_ADMIN = 4
};

struct UserSession {
    char session_id[MAX_SESSION_ID];
    char username[MAX_USERNAME];
    time_t creation_time;
    time_t last_access;
    int permissions;
};

struct UserProfile {
    char username[MAX_USERNAME];
    char email[256];
    char title[128];
    char phone[32];
    char pronouns[32];
    char responsibility[256];
    time_t last_login;
    int groups[MAX_GROUPS];
    int group_count;
    int permissions;
};

struct PasswdEntry {
    char username[MAX_USERNAME];
    char password_hash[128];
    uid_t uid;
    gid_t gid;
    char full_name[256];
    char home_dir[PATH_MAX];
    char shell[128];
};

int authInit(const char *passwd_file);
int authValidateUser(const char *username, const char *password);
int authCreateSession(const char *username, struct UserSession *session);
int authValidateSession(const char *session_id);
int authGetUserProfile(const char *username, struct UserProfile *profile);
int authLoadPasswdEntry(const char *username, struct PasswdEntry *entry);
int authSavePasswdEntry(const struct PasswdEntry *entry);
int authVerifyPassword(const char *password, const char *hash);
void authCleanup(void);

int authUpdateUserProfile(const char *username, const struct UserProfile *profile);
int authSaveUserProfile(const struct UserProfile *profile);

#endif /* AUTH_H */
