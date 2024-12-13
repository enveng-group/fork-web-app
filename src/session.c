/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "../include/session.h"
#include "../include/audit.h"
#include "../include/logger.h"
#include "../include/data_structures.h"

static struct SessionStore store;
static pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;

static void generateSessionId(char* session_id)
{
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int i;

    for (i = 0; i < SESSION_ID_LEN - 1; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        session_id[i] = charset[key];
    }
    session_id[SESSION_ID_LEN - 1] = '\0';
}

int sessionInit(void)
{
    memset(&store, 0, sizeof(struct SessionStore));
    srand((unsigned int)time(NULL));
    return 0;
}

char* sessionCreate(uid_t uid, int permissions)
{
    struct Session* session;
    static char session_id[SESSION_ID_LEN];
    time_t now;

    pthread_mutex_lock(&session_mutex);

    if (store.count >= MAX_SESSIONS) {
        pthread_mutex_unlock(&session_mutex);
        return NULL;
    }

    session = &store.sessions[store.count++];
    generateSessionId(session_id);
    now = time(NULL);

    strcpy(session->id, session_id);
    session->uid = uid;
    session->created = now;
    session->last_access = now;
    session->permissions = permissions;

    pthread_mutex_unlock(&session_mutex);
    return session_id;
}

int sessionValidate(const char* session_id)
{
    int i;
    time_t now;

    if (!session_id) {
        return -1;
    }

    now = time(NULL);
    pthread_mutex_lock(&session_mutex);

    for (i = 0; i < store.count; i++) {
        if (strcmp(store.sessions[i].id, session_id) == 0) {
            if ((now - store.sessions[i].last_access) > SESSION_TIMEOUT) {
                pthread_mutex_unlock(&session_mutex);
                return -1;
            }
            store.sessions[i].last_access = now;
            pthread_mutex_unlock(&session_mutex);
            return 0;
        }
    }

    pthread_mutex_unlock(&session_mutex);
    return -1;
}

void sessionDestroy(const char* session_id)
{
    int i, j;

    pthread_mutex_lock(&session_mutex);

    for (i = 0; i < store.count; i++) {
        if (strcmp(store.sessions[i].id, session_id) == 0) {
            for (j = i; j < store.count - 1; j++) {
                store.sessions[j] = store.sessions[j + 1];
            }
            store.count--;
            break;
        }
    }

    pthread_mutex_unlock(&session_mutex);
}

void sessionCleanup(void)
{
    int i, j;
    time_t now;

    now = time(NULL);
    pthread_mutex_lock(&session_mutex);

    for (i = 0; i < store.count; i++) {
        if ((now - store.sessions[i].last_access) > SESSION_TIMEOUT) {
            for (j = i; j < store.count - 1; j++) {
                store.sessions[j] = store.sessions[j + 1];
            }
            store.count--;
            i--;
        }
    }

    pthread_mutex_unlock(&session_mutex);
}

int sessionLogout(const char* session_id, uid_t uid, const char* username)
{
    int found = 0;
    struct AuditEntry audit;
    int i, j; /* Declare loop variables */

    pthread_mutex_lock(&session_mutex);

    /* Find and remove session */
    for (i = 0; i < store.count; i++) {
        if (strcmp(store.sessions[i].id, session_id) == 0) {
            found = 1;
            /* Log logout event */
            memset(&audit, 0, sizeof(struct AuditEntry));
            audit.timestamp = time(NULL);
            audit.user_id = uid;
            strncpy(audit.username, username, sizeof(audit.username) - 1);
            audit.event_type = AUDIT_LOGOUT;
            snprintf(audit.message, sizeof(audit.message),
                    "User logged out: %s", username);
            auditLog(&audit);

            /* Remove session */
            for (j = i; j < store.count - 1; j++) {
                store.sessions[j] = store.sessions[j + 1];
            }
            store.count--;
            break;
        }
    }

    pthread_mutex_unlock(&session_mutex);
    return found ? 0 : -1;
}

int sessionValidateWithAudit(const char* session_id, uid_t uid, const char* username)
{
    int i, result = -1;
    time_t now;
    struct AuditEntry audit;

    if (!session_id) {
        return -1;
    }

    now = time(NULL);
    memset(&audit, 0, sizeof(struct AuditEntry));
    audit.timestamp = now;
    audit.user_id = uid;
    strncpy(audit.username, username, sizeof(audit.username) - 1);

    pthread_mutex_lock(&session_mutex);

    for (i = 0; i < store.count; i++) {
        if (strcmp(store.sessions[i].id, session_id) == 0) {
            if ((now - store.sessions[i].last_access) > SESSION_TIMEOUT) {
                audit.event_type = AUDIT_SESSION_EXPIRED;
                snprintf(audit.message, sizeof(audit.message),
                        "Session expired: %s", session_id);
                result = -1;
            } else {
                store.sessions[i].last_access = now;
                audit.event_type = AUDIT_SESSION_VALIDATED;
                snprintf(audit.message, sizeof(audit.message),
                        "Session validated: %s", session_id);
                result = 0;
            }
            break;
        }
    }

    pthread_mutex_unlock(&session_mutex);

    auditLog(&audit);
    return result;
}

int sessionValidateWithResponse(const char *session_id)
{
    struct Response *response;
    int result;

    result = sessionValidate(session_id);
    if (result != 0) {
        response = dsCreateResponse(401); /* Use numeric code instead of constant */
        if (response) {
            dsAddResponseHeader(response, "Content-Type", "application/json");
            /* Add session error details */
            dsFreeResponse(response);
        }
        return -1;
    }

    return 0;
}
