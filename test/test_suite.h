/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* filepath: /devcontainer/web-app/test/test_suite.h */
#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#define CACHE_API_VERSION 1
#define MEM_API_VERSION 1

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
void test_init_system(void);  /* Add missing prototype */

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

/* Test case declarations for process */
void test_process_create(void);
void test_process_terminate(void);
void test_process_invalid_create(void);
void test_process_resource_cleanup(void); /* Add this */

/* Test case declarations for scheduler */
void test_scheduler_init(void);
void test_scheduler_start_stop(void);
void test_scheduler_add_task(void);
void test_scheduler_remove_task(void);
void test_scheduler_execution(void);
/* Test case declarations for scheduler states */
void test_scheduler_state_transitions(void);
void test_scheduler_pause_resume(void);
void test_scheduler_high_concurrency(void); /* Add this */

/* Test case declarations for main */
void test_main_startup(void);
void test_main_signal_handling(void);
void test_main_cleanup(void);
void test_main_args_handling(void);
void test_memory_management(void);  /* Add this */
void test_cache_operations(void);   /* Add this */

/* Test case declarations for mem */
void test_mem_init(void);
void test_mem_alloc(void);
void test_mem_free(void);
void test_mem_stress(void);
void test_mem_edge_cases(void);     /* Add this */
void test_mem_fragmentation(void);  /* Add this */
void test_mem_multi_pool(void);     /* Add this */
void test_mem_stress_allocation(void); /* Add stress test declaration */
void test_mem_corruption_detection(void); /* Add this */
void test_system_memory_stress(void);  /* System-wide memory stress test */
void test_memory_stress(void);         /* Unit test for memory subsystem */

/* Test case declarations for cache */
void test_cache_init(void);
void test_cache_set_get(void);
void test_cache_delete(void);
void test_cache_capacity(void);
void test_cache_lru(void);
void test_cache_lfu(void);
void test_cache_edge_cases(void);              /* Add this */
void test_cache_eviction_policy(void);         /* Add this */
void test_cache_concurrent_access(void);        /* Add this */
void test_cache_performance(void);             /* Add this */
void test_cache_stress(void); /* Add stress test declaration */
void test_cache_thread_safety(void); /* Add this */
void test_cache_eviction(void); /* Add this */

/* Test case declarations for fs */
void test_fs_error_conditions(void); /* Add this */

/* Test case declaration for consistency */
void test_path_consistency(void);
void test_buffer_sizes(void);
void test_version_consistency(void);
void test_env_consistency(void);
void test_path_validation(void);
void test_path_normalization(void);

/* Integration test declarations */
void test_system_integration(void);
void test_fs_cache_integration(void);
void test_logging_error_integration(void);
void test_config_env_integration(void);
void test_scheduler_process_mem_integration(void);

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
int test_process(void);
int test_scheduler(void);
int test_mem(void);
int test_cache(void);
int init_consistency_suite(void);
int init_path_validation_suite(void);

/* Test suite initialization functions */
int test_mem(void);
int test_cache(void);

/* Add to test_suite.h */
#define ASSERT_MEM_STATUS(expected) \
    do { \
        enum mem_status actual = memGetStatus(); \
        if (actual != expected) { \
            printf("Expected status %d, got %d at %s:%d\n", \
                   expected, actual, __FILE__, __LINE__); \
            CU_ASSERT_EQUAL(actual, expected); \
        } \
    } while (0)

void test_scheduler_stress(void); /* Add stress test declaration */

/* Socket test case declarations */
void test_socket_init(void);
void test_socket_error_str(void);
void test_socket_init_flags(void);
void test_socket_cleanup(void);
void test_socket_tcp_operations(void);
void test_socket_udp_operations(void);
void test_socket_nonblock(void);
void test_socket_reuse_addr(void);
void test_socket_integration(void);

/* Add to suite initialization functions */
int test_socket(void);

#endif /* TEST_SUITE_H */
