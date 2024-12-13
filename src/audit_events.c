/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "../include/audit_events.h"
#include "../include/audit.h"
#include "../include/session.h"

void logSessionEvent(const char* session_id, uid_t uid,
                    const char* username, enum AuditEventType event_type)
{
    struct AuditEntry entry;
    char msg[AUDIT_MAX_MSG_LEN];

    if (!session_id || !username) {
        return;
    }

    memset(&entry, 0, sizeof(struct AuditEntry));
    entry.timestamp = time(NULL);
    entry.user_id = uid;
    strncpy(entry.username, username, AUDIT_USERNAME_LEN - 1);
    entry.event_type = event_type;

    switch(event_type) {
        case AUDIT_SESSION_CREATED:
            snprintf(msg, sizeof(msg), "Session created: %s", session_id);
            break;
        case AUDIT_SESSION_VALIDATED:
            snprintf(msg, sizeof(msg), "Session validated: %s", session_id);
            break;
        case AUDIT_SESSION_EXPIRED:
            snprintf(msg, sizeof(msg), "Session expired: %s", session_id);
            break;
        case AUDIT_SESSION_DESTROYED:
            snprintf(msg, sizeof(msg), "Session destroyed: %s", session_id);
            break;
        default:
            snprintf(msg, sizeof(msg), "Unknown event for session: %s", session_id);
    }

    strncpy(entry.message, msg, AUDIT_MAX_MSG_LEN - 1);
    auditLog(&entry);
}
