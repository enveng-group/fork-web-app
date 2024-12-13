/**
 * Copyright 2024 Enveng Group - Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* database.h */
#ifndef DATABASE_H
#define DATABASE_H

#include <sys/types.h>

/* Database record types */
enum RecordType {
    REC_PROJECT,
    REC_ACCOUNTABILITY,
    REC_ACTION_STATUS,
    REC_ENV_ASPECT,
    REC_ENV_MECHANISM,
    REC_INSPECTION_FREQ,
    REC_INSPECTION_YN,
    REC_PROJECT_PHASE,
    REC_RECURRING_FREQ,
    REC_RECURRING_YN,
    REC_RECURRING_STATUS
};

/* Database connection handle */
struct DBHandle {
    FILE *fp;
    char *filename;
    pthread_mutex_t mutex;
    int is_open;
};

/* Project record structure */
struct ProjectRecord {
    char project_name[256];
    char mechanism[256];
    char procedure[256];
    char aspect[256];
    char obligation_num[256];
    char obligation[1001];
    char accountability[256];
    char responsibility[256];
    char phase[256];
    char status[256];
    char due_date[32];
    char close_date[32];
    int recurring;
    int inspection_required;
};

/* Function prototypes */
int dbInit(const char *filename, struct DBHandle *handle);
void dbCleanup(struct DBHandle *handle);
int dbReadProject(struct DBHandle *handle, const char *obligation_num, struct ProjectRecord *record);
int dbUpdateProject(struct DBHandle *handle, const struct ProjectRecord *record);
int dbListProjects(struct DBHandle *handle, struct ProjectRecord **records, size_t *count);
int dbSearchProjects(struct DBHandle *handle, const char *query, struct ProjectRecord **records, size_t *count);

#endif /* DATABASE_H */
