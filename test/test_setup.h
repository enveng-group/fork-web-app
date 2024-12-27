/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: /home/appuser/fork-web-app/test/setup.h */
#ifndef TEST_SETUP_H
#define TEST_SETUP_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#undef AUTH_FILE
#undef AUDIT_LOG
#undef W6946_REC

#define TEST_AUTH_FILE "test/auth.passwd"
#define TEST_AUDIT_LOG "test/audit.log"
#define TEST_W6946_REC "test/w6946.rec"

/* Function prototype - remove static keyword to make it visible */
int test_setup_environment(void);

#endif /* TEST_SETUP_H */
