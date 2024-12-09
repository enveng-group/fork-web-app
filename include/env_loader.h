/**
 * \file env_loader.h
 * \brief Header file for environment variable loading functions.
 * \author Adrian Gallo
 * \copyright 2024 Enveng Group
 * \license AGPL-3.0-or-later
 */

#ifndef ENV_LOADER_H
#define ENV_LOADER_H


#define SERVER_IP
#define SERVER_PORT
/**
 * \struct env_var_t
 * \brief Structure to hold environment variables.
 */
typedef struct
{
    char *key;   /**< Key of the environment variable */
    char *value; /**< Value of the environment variable */
} env_var_t;

/* Function declarations */

/**
 * \brief Loads environment variables from the environment file.
 */
void loadEnvironmentVariables (void);

/**
 * \brief Gets the value of an environment variable.
 *
 * \param name Name of the environment variable.
 * \return Value of the environment variable, or NULL if not found.
 */
const char *getEnvVariable (const char *name);

/**
 * \brief Trims leading and trailing whitespace from a string.
 *
 * \param str String to trim.
 * \return Pointer to the trimmed string.
 */
char *trimWhitespace (char *str);

/**
 * \brief Loads environment configuration from a file.
 *
 * \param filename Name of the file to load.
 * \return 0 if loading is successful, otherwise an error code.
 */
int loadEnvConfig (const char *filename);

#endif /* ENV_LOADER_H */
