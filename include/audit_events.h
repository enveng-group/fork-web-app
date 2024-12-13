/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
#ifndef AUDIT_EVENTS_H
#define AUDIT_EVENTS_H

enum AuditEventType {
    AUDIT_SESSION_CREATED = 1,
    AUDIT_SESSION_VALIDATED,
    AUDIT_SESSION_EXPIRED,
    AUDIT_SESSION_DESTROYED,
    AUDIT_LOGIN_SUCCESS,
    AUDIT_LOGIN_FAILURE,
    AUDIT_LOGOUT
};

#endif /* AUDIT_EVENTS_H */
