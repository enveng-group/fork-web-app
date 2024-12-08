/**
 * \file records.c
 * \brief Implements record handling functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/records.h"
#include "../include/logger.h"

/**
 * \brief Initializes a Project structure.
 *
 * \param project Pointer to the Project structure to initialize.
 */
void initProject(Project *project) {
    memset(project, 0, sizeof(Project));
}

/**
 * \brief Prints a Project structure.
 *
 * \param project Pointer to the Project structure to print.
 */
void printProject(const Project *project) {
    printf("Project Name: %s\n", project->project_name);
    printf("Primary Environmental Mechanism: %s\n", project->primary_environmental_mechanism);
    printf("Procedure: %s\n", project->procedure);
    printf("Environmental Aspect: %s\n", project->environmental_aspect);
    printf("Obligation Number: %s\n", project->obligation_number);
    printf("Obligation: %s\n", project->obligation);
    printf("Accountability: %s\n", project->accountability);
    printf("Responsibility: %s\n", project->responsibility);
    printf("Project Phase: %s\n", project->project_phase);
    printf("Action Due Date: %s\n", project->action_due_date);
    printf("Close Out Date: %s\n", project->close_out_date);
    printf("Status: %s\n", project->status);
    printf("Supporting Information: %s\n", project->supporting_information);
    printf("General Comments: %s\n", project->general_comments);
    printf("Compliance Comments: %s\n", project->compliance_comments);
    printf("Non-Conformance Comments: %s\n", project->non_conformance_comments);
    printf("Evidence: %s\n", project->evidence);
    printf("Person Email: %s\n", project->person_email);
    printf("Recurring Obligation: %s\n", project->recurring_obligation);
    printf("Recurring Frequency: %d\n", project->recurring_frequency);
    printf("Recurring Status: %s\n", project->recurring_status);
    printf("Recurring Forecasted Date: %s\n", project->recurring_forecasted_date);
    printf("Inspection: %s\n", project->inspection);
    printf("Inspection Frequency: %d\n", project->inspection_frequency);
    printf("Site or Desktop: %s\n", project->site_or_desktop);
    printf("New Control Action Required: %s\n", project->new_control_action_required);
    printf("Obligation Type: %s\n", project->obligation_type);
}

/**
 * \brief Loads a Project structure from a .rec file.
 *
 * \param filename Name of the file to load.
 * \param project Pointer to the Project structure to load.
 * \return 0 on success, otherwise an error code.
 */
int loadProjectFromFile(const char *filename, Project *project) {
    FILE *file = fopen(filename, "r");
    char line[2048];
    char key[256], value[1024];

    if (!file) {
        logError("Failed to open file: %s", filename);
        return -1;
    }

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%255[^:]: %1023[^\n]", key, value) == 2) {
            if (strcmp(key, "Project Name") == 0) {
                strncpy(project->project_name, value, sizeof(project->project_name) - 1);
                project->project_name[sizeof(project->project_name) - 1] = '\0';
            } else if (strcmp(key, "Primary Environmental Mechanism") == 0) {
                strncpy(project->primary_environmental_mechanism, value, sizeof(project->primary_environmental_mechanism) - 1);
                project->primary_environmental_mechanism[sizeof(project->primary_environmental_mechanism) - 1] = '\0';
            } else if (strcmp(key, "Procedure") == 0) {
                strncpy(project->procedure, value, sizeof(project->procedure) - 1);
                project->procedure[sizeof(project->procedure) - 1] = '\0';
            } else if (strcmp(key, "Environmental Aspect") == 0) {
                strncpy(project->environmental_aspect, value, sizeof(project->environmental_aspect) - 1);
                project->environmental_aspect[sizeof(project->environmental_aspect) - 1] = '\0';
            } else if (strcmp(key, "Obligation Number") == 0) {
                strncpy(project->obligation_number, value, sizeof(project->obligation_number) - 1);
                project->obligation_number[sizeof(project->obligation_number) - 1] = '\0';
            } else if (strcmp(key, "Obligation") == 0) {
                strncpy(project->obligation, value, sizeof(project->obligation) - 1);
                project->obligation[sizeof(project->obligation) - 1] = '\0';
            } else if (strcmp(key, "Accountability") == 0) {
                strncpy(project->accountability, value, sizeof(project->accountability) - 1);
                project->accountability[sizeof(project->accountability) - 1] = '\0';
            } else if (strcmp(key, "Responsibility") == 0) {
                strncpy(project->responsibility, value, sizeof(project->responsibility) - 1);
                project->responsibility[sizeof(project->responsibility) - 1] = '\0';
            } else if (strcmp(key, "Project Phase") == 0) {
                strncpy(project->project_phase, value, sizeof(project->project_phase) - 1);
                project->project_phase[sizeof(project->project_phase) - 1] = '\0';
            } else if (strcmp(key, "Action Due Date") == 0) {
                strncpy(project->action_due_date, value, sizeof(project->action_due_date) - 1);
                project->action_due_date[sizeof(project->action_due_date) - 1] = '\0';
            } else if (strcmp(key, "Close Out Date") == 0) {
                strncpy(project->close_out_date, value, sizeof(project->close_out_date) - 1);
                project->close_out_date[sizeof(project->close_out_date) - 1] = '\0';
            } else if (strcmp(key, "Status") == 0) {
                strncpy(project->status, value, sizeof(project->status) - 1);
                project->status[sizeof(project->status) - 1] = '\0';
            } else if (strcmp(key, "Supporting Information") == 0) {
                strncpy(project->supporting_information, value, sizeof(project->supporting_information) - 1);
                project->supporting_information[sizeof(project->supporting_information) - 1] = '\0';
            } else if (strcmp(key, "General Comments") == 0) {
                strncpy(project->general_comments, value, sizeof(project->general_comments) - 1);
                project->general_comments[sizeof(project->general_comments) - 1] = '\0';
            } else if (strcmp(key, "Compliance Comments") == 0) {
                strncpy(project->compliance_comments, value, sizeof(project->compliance_comments) - 1);
                project->compliance_comments[sizeof(project->compliance_comments) - 1] = '\0';
            } else if (strcmp(key, "Non-Conformance Comments") == 0) {
                strncpy(project->non_conformance_comments, value, sizeof(project->non_conformance_comments) - 1);
                project->non_conformance_comments[sizeof(project->non_conformance_comments) - 1] = '\0';
            } else if (strcmp(key, "Evidence") == 0) {
                strncpy(project->evidence, value, sizeof(project->evidence) - 1);
                project->evidence[sizeof(project->evidence) - 1] = '\0';
            } else if (strcmp(key, "Person Email") == 0) {
                strncpy(project->person_email, value, sizeof(project->person_email) - 1);
                project->person_email[sizeof(project->person_email) - 1] = '\0';
            } else if (strcmp(key, "Recurring Obligation") == 0) {
                strncpy(project->recurring_obligation, value, sizeof(project->recurring_obligation) - 1);
                project->recurring_obligation[sizeof(project->recurring_obligation) - 1] = '\0';
            } else if (strcmp(key, "Recurring Frequency") == 0) {
                project->recurring_frequency = atoi(value);
            } else if (strcmp(key, "Recurring Status") == 0) {
                strncpy(project->recurring_status, value, sizeof(project->recurring_status) - 1);
                project->recurring_status[sizeof(project->recurring_status) - 1] = '\0';
            } else if (strcmp(key, "Recurring Forecasted Date") == 0) {
                strncpy(project->recurring_forecasted_date, value, sizeof(project->recurring_forecasted_date) - 1);
                project->recurring_forecasted_date[sizeof(project->recurring_forecasted_date) - 1] = '\0';
            } else if (strcmp(key, "Inspection") == 0) {
                strncpy(project->inspection, value, sizeof(project->inspection) - 1);
                project->inspection[sizeof(project->inspection) - 1] = '\0';
            } else if (strcmp(key, "Inspection Frequency") == 0) {
                project->inspection_frequency = atoi(value);
            } else if (strcmp(key, "Site or Desktop") == 0) {
                strncpy(project->site_or_desktop, value, sizeof(project->site_or_desktop) - 1);
                project->site_or_desktop[sizeof(project->site_or_desktop) - 1] = '\0';
            } else if (strcmp(key, "New Control Action Required") == 0) {
                strncpy(project->new_control_action_required, value, sizeof(project->new_control_action_required) - 1);
                project->new_control_action_required[sizeof(project->new_control_action_required) - 1] = '\0';
            } else if (strcmp(key, "Obligation Type") == 0) {
                strncpy(project->obligation_type, value, sizeof(project->obligation_type) - 1);
                project->obligation_type[sizeof(project->obligation_type) - 1] = '\0';
            }
        }
    }

    fclose(file);
    return 0;
}
