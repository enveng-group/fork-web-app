/**
 * \file csv_utils.h
 * \brief Header file for CSV utilities.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include "project.h"

/**
 * \brief Parses a CSV file into a Project structure.
 *
 * \param csv_filename Name of the CSV file.
 * \param project Pointer to the Project structure.
 * \return 0 on success, otherwise an error code.
 */
int parseCsv (const char *csv_filename, Project *project);

/**
 * \brief Writes a Project structure to a CSV file.
 *
 * \param csv_filename Name of the CSV file.
 * \param project Pointer to the Project structure.
 * \return 0 on success, otherwise an error code.
 */
int writeCsv (const char *csv_filename, const Project *project);

int csv_read(const char *filename);
int csv_write(const char *filename, const void *data);

#endif /* CSV_UTILS_H */
