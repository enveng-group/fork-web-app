/**
 * \file project.h
 * \brief Header file for Project structure and functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef PROJECT_H
#define PROJECT_H

#include <stddef.h> /* For size_t */

/**
 * \struct Project
 * \brief Structure to hold project information.
 */
typedef struct {
    char project_name[256];
    char primary_environmental_mechanism[256];
    char procedure[256];
    char environmental_aspect[256];
    char obligation_number[256];
    char obligation[1001];
    char accountability[256];
    char responsibility[256];
    char project_phase[256];
    char action_due_date[20];
    char close_out_date[20];
    char status[256];
    char supporting_information[1001];
    char general_comments[1001];
    char compliance_comments[1001];
    char non_conformance_comments[1001];
    char evidence[1001];
    char person_email[256];
    char recurring_obligation[4];
    int  recurring_frequency;
    char recurring_status[9];
    char recurring_forecasted_date[20];
    char inspection[4];
    int  inspection_frequency;
    char site_or_desktop[8];
    char new_control_action_required[256];
    char obligation_type[256];
    int  field_count;  /**< Example field to avoid unused parameter warning */
} Project;

/* Function prototypes */
void initProject(Project *project);
void printProject(const Project *project);
int loadProjectFromFile(const char *filename, Project *project);

#endif /* PROJECT_H */
