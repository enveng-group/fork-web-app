/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef SESSION_H
#define SESSION_H

#include <time.h>
#include <sys/types.h>

#define SESSION_ID_LEN 64
#define MAX_SESSIONS 1000
#define SESSION_TIMEOUT 3600

struct Session {
    char id[SESSION_ID_LEN];
    uid_t uid;
    time_t created;
    time_t last_access;
    int permissions;
};

struct SessionStore {
    struct Session sessions[MAX_SESSIONS];
    int count;
};

int sessionInit(void);
char* sessionCreate(uid_t uid, int permissions);
int sessionValidate(const char* session_id);
void sessionDestroy(const char* session_id);
int sessionUpdate(const char* session_id);
void sessionCleanup(void);
int sessionLogout(const char* session_id, uid_t uid, const char* username);
int sessionValidateWithAudit(const char* session_id, uid_t uid, const char* username);

#endif /* SESSION_H */
