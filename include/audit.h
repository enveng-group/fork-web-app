/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef AUDIT_H
#define AUDIT_H

#include <time.h>
#include <sys/types.h>
#include "audit_events.h"

#define AUDIT_MAX_MSG_LEN 256
#define AUDIT_USERNAME_LEN 32

/* Audit entry structure */
struct AuditEntry {
    time_t timestamp;
    uid_t user_id;
    char username[AUDIT_USERNAME_LEN];
    enum AuditEventType event_type;
    char message[AUDIT_MAX_MSG_LEN];
};

/* Function prototypes */
void logSessionEvent(const char* session_id, uid_t uid,
                    const char* username, enum AuditEventType event_type);
int auditInit(const char *audit_path);
void auditLog(const struct AuditEntry* entry);

void auditCleanup(void);

#endif /* AUDIT_H */
