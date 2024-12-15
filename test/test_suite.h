/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: /devcontainer/web-app/test/test_suite.h */
#ifndef TEST_SUITE_H
#define TEST_SUITE_H

/* Test case declarations for app_error */
void test_app_error_init_null(void);
void test_app_error_logging(void);
void test_app_error_levels(void);

/* Test case declarations for config */
void test_config_init_null(void);
void test_config_init_valid(void);
void test_config_load(void);
void test_config_get_valid(void);
void test_config_get_invalid(void);
void test_config_cleanup(void);

/* Test case declarations for constants */
void test_constants_init(void);
void test_constants_app_values(void);
void test_constants_log_values(void);
void test_constants_db_values(void);
void test_constants_cache_values(void);

/* Test case declarations for env */
void test_env_init_null(void);
void test_env_init_valid(void);
void test_env_get_valid(void);
void test_env_get_invalid(void);
void test_env_set_valid(void);
void test_env_set_invalid(void);
void test_env_cleanup(void);

/* Test case declarations for fs */
void test_fs_init_null(void);
void test_fs_init_valid(void);
void test_fs_create_file(void);
void test_fs_delete_file(void);
void test_fs_read_file(void);
void test_fs_write_file(void);
void test_fs_invalid_operations(void);

/* Test case declarations for init */
void test_system_initialization(void);
void test_config_loading(void);
void test_system_shutdown(void);

/* Test case declarations for logging */
void test_log_init_null(void);
void test_log_init_valid(void);
void test_log_write(void);
void test_log_rotate(void);
void test_log_levels(void);
void test_log_cleanup(void);

/* Test case declarations for shell */
void test_shell_execute_command(void);
void test_shell_invalid_command(void);

/* Test case declarations for main */
void test_main_startup(void);
void test_main_signal_handling(void);
void test_main_cleanup(void);
void test_main_args_handling(void);

/* Suite initialization functions */
int test_app_error(void);
int test_config(void);
int test_constants(void);
int test_env(void);
int test_fs(void);
int test_init(void);
int test_logging(void);
int test_shell(void);
int test_main_module(void);

#endif /* TEST_SUITE_H */
