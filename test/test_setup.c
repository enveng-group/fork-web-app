/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: /home/appuser/fork-web-app/test/setup.c */
#include "test_setup.h"

/* In test_setup.c */
int
test_setup_environment(void)
{
    FILE *fp;

    /* Create test directories */
    if (mkdir("test", 0755) != 0 && errno != EEXIST) {
        return -1;
    }

    /* Create test auth.passwd with known credentials */
    fp = fopen(TEST_AUTH_FILE, "w");
    if (fp) {
        fprintf(fp, "admin:x:0:0:Admin User:/root:/bin/sh:1\n");
        fprintf(fp, "testuser:x:1000:1000:Test User:/home/testuser:/bin/sh:0\n");
        fclose(fp);
    }

    /* Create test record file */
    fp = fopen(TEST_W6946_REC, "w");
    if (fp) {
        fprintf(fp, "Test Record Data\n");
        fclose(fp);
    }

    /* Set proper permissions */
    chmod(TEST_AUTH_FILE, 0644);
    chmod(TEST_W6946_REC, 0644);

    return 0;
}
