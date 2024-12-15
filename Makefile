# Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
# SPDX-License-Identifier: 	AGPL-3.0-or-later
# Makefile

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = test

# Compiler and flags
CC = gcc
CFLAGS = -std=c90 -D_POSIX_C_SOURCE=1 -D_XOPEN_SOURCE=500 -Wall -Wextra -pedantic
INCLUDES = -I./include -I./src -I/usr/include
LDFLAGS = -L/usr/lib

# Main program flags (static linking)
MAIN_LDFLAGS = $(LDFLAGS) -static -pthread

# Test flags (dynamic linking for CUnit)
TEST_LDFLAGS = $(LDFLAGS) -pthread -lcunit

# Main program sources and objects
MAIN_SRC = $(SRC_DIR)/main.c
LIB_SRCS = $(filter-out $(MAIN_SRC),$(wildcard $(SRC_DIR)/*.c))
LIB_OBJS = $(LIB_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
MAIN_OBJ = $(MAIN_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Test sources and objects (excluding main program objects)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_LIB_OBJS = $(filter-out $(MAIN_OBJ),$(LIB_OBJS))

# Test module objects
TEST_APP_ERROR_OBJ = $(BUILD_DIR)/test_app_error.o
TEST_CONFIG_OBJ = $(BUILD_DIR)/test_config.o
TEST_CONSTANTS_OBJ = $(BUILD_DIR)/test_constants.o
TEST_ENV_OBJ = $(BUILD_DIR)/test_env.o
TEST_FS_OBJ = $(BUILD_DIR)/test_fs.o
TEST_INIT_OBJ = $(BUILD_DIR)/test_init.o
TEST_LOGGING_OBJ = $(BUILD_DIR)/test_logging.o
TEST_SHELL_OBJ = $(BUILD_DIR)/test_shell.o
TEST_MAIN_MODULE_OBJ = $(BUILD_DIR)/test_main_module.o
TEST_MAIN_OBJ = $(BUILD_DIR)/test_main.o

# Targets
MAIN_TARGET = $(BIN_DIR)/enssol
TEST_TARGET = $(BIN_DIR)/test_runner

# Default target
all: $(MAIN_TARGET)

# Directory creation targets
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Main program build
$(MAIN_TARGET): $(MAIN_OBJ) $(LIB_OBJS) | $(BIN_DIR)
	$(CC) $(MAIN_OBJ) $(LIB_OBJS) $(MAIN_LDFLAGS) -o $@

# Test build
$(TEST_TARGET): $(TEST_APP_ERROR_OBJ) $(TEST_CONFIG_OBJ) $(TEST_CONSTANTS_OBJ) \
				$(TEST_ENV_OBJ) $(TEST_FS_OBJ) $(TEST_INIT_OBJ) \
				$(TEST_LOGGING_OBJ) $(TEST_SHELL_OBJ) $(TEST_MAIN_MODULE_OBJ) \
				$(TEST_MAIN_OBJ) $(TEST_LIB_OBJS) | $(BIN_DIR)
	$(CC) $^ $(TEST_LDFLAGS) -o $@

# Object file compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build object file with directory creation
build/test_init.o: test/test_init.c include/init.h include/app_error.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Explicit test dependencies
$(TEST_APP_ERROR_OBJ): $(TEST_DIR)/test_app_error.c include/app_error.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_CONFIG_OBJ): $(TEST_DIR)/test_config.c include/config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_CONSTANTS_OBJ): $(TEST_DIR)/test_constants.c include/constants.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_ENV_OBJ): $(TEST_DIR)/test_env.c include/env.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_FS_OBJ): $(TEST_DIR)/test_fs.c include/fs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_INIT_OBJ): $(TEST_DIR)/test_init.c include/init.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_LOGGING_OBJ): $(TEST_DIR)/test_logging.c include/logging.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_SHELL_OBJ): $(TEST_DIR)/test_shell.c include/shell.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_MAIN_MODULE_OBJ): $(TEST_DIR)/test_main_module.c include/init.h include/fs.h include/shell.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_MAIN_OBJ): $(TEST_DIR)/test_main.c test/test_suite.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Test target with setup and run
test: $(BUILD_DIR) $(BIN_DIR) $(TEST_TARGET)
	mkdir -p test/fs_test
	./$(TEST_TARGET)
	rm -rf test/fs_test

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean test $(BUILD_DIR) $(BIN_DIR)
