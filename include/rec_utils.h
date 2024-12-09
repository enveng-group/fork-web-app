/**
 * \file rec_utils.h
 * \brief Header file for REC file utilities.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef REC_UTILS_H
#define REC_UTILS_H

#include "project.h"  /* Include the header where Project is defined */

/**
 * \brief Converts a CSV file to a REC file.
 *
 * \param csv_filename Name of the CSV file.
 * \param rec_filename Name of the REC file.
 * \return 0 on success, otherwise an error code.
 */
int csvToRec(const char *csv_filename, const char *rec_filename);

/**
 * \brief Converts a REC file to a CSV file.
 *
 * \param rec_filename Name of the REC file.
 * \param csv_filename Name of the CSV file.
 * \return 0 on success, otherwise an error code.
 */
int recToCsv(const char *rec_filename, const char *csv_filename);

/**
 * \brief Deletes a record from a REC file.
 *
 * \param rec_filename Name of the REC file.
 * \param record_id ID of the record to delete.
 * \return 0 on success, otherwise an error code.
 */
int deleteRec(const char *rec_filename, const char *record_id);

/**
 * \brief Fixes a REC file.
 *
 * \param rec_filename Name of the REC file.
 * \return 0 on success, otherwise an error code.
 */
int fixRec(const char *rec_filename);

/**
 * \brief Formats a REC file.
 *
 * \param rec_filename Name of the REC file.
 * \return 0 on success, otherwise an error code.
 */
int formatRec(const char *rec_filename);

/**
 * \brief Inserts a record into a REC file.
 *
 * \param rec_filename Name of the REC file.
 * \param record Pointer to the record to insert.
 * \return 0 on success, otherwise an error code.
 */
int insertRec(const char *rec_filename, const Project *record);

/**
 * \brief Selects records from a REC file.
 *
 * \param rec_filename Name of the REC file.
 * \param criteria Selection criteria.
 * \return 0 on success, otherwise an error code.
 */
int selectRec(const char *rec_filename, const char *criteria);

/**
 * \brief Sets a field in a REC file.
 *
 * \param rec_filename Name of the REC file.
 * \param record_id ID of the record to update.
 * \param field_name Name of the field to set.
 * \param field_value Value of the field to set.
 * \return 0 on success, otherwise an error code.
 */
int setRecField(const char *rec_filename, const char *record_id, const char *field_name, const char *field_value);

#endif /* REC_UTILS_H */
