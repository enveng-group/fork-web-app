/**
 * \file csv_utils.c
 * \brief Implementation of CSV utilities.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#include "../include/csv_utils.h"
#include "../include/logger.h"
#include "../include/project.h" /* Ensure this header includes the definition of Project */
#include <stdio.h>  /* Include stdio.h for FILE, fopen, fgets, fclose, fprintf */

/**
 * \brief Parses a CSV file into a Project structure.
 *
 * \param csv_filename Name of the CSV file.
 * \param project Pointer to the Project structure.
 * \return 0 on success, otherwise an error code.
 */
int parseCsv(const char *csv_filename, Project *project)
{
    FILE *file;
    char line[1024];  /* Move declaration to the beginning */

    file = fopen(csv_filename, "r");
    if (file == NULL)
    {
        loggerLog("Failed to open CSV file");
        return -1;
    }

    while (fgets(line, sizeof(line), file))
    {
        /* Basic usage of project to avoid unused parameter warning */
        project->field_count++;
        /* Parsing logic here */
    }

    fclose(file);
    return 0;
}

/**
 * \brief Writes data to a CSV file.
 *
 * \param csv_filename The name of the CSV file.
 * \param project The project structure containing data to write.
 * \return 0 on success, otherwise an error code.
 */
int writeCsv(const char *csv_filename, const Project *project)
{
    FILE *file;

    file = fopen(csv_filename, "w");
    if (file == NULL)
    {
        loggerLog("Failed to open CSV file for writing");
        return -1;
    }

    /* Example: Write project data to CSV */
    fprintf(file, "Field Count: %d\n", project->field_count);

    fclose(file);
    return 0;
}
