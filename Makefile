# Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
# SPDX-License-Identifier: AGPL-3.0-or-later

# Directory structure
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = test

# Compiler and basic flags
CC = gcc
CFLAGS = -std=c90 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -Wall \
-ansi -Wextra -pedantic -Werror -Wshadow -Wconversion \
-Wstrict-prototypes -Wmissing-prototypes \
-fanalyzer \
-fstack-protector-strong -fstack-check \
-fdata-sections -ffunction-sections \
-fno-common -fstrict-aliasing \
-Warray-bounds -Wstack-protector \
-Wformat=2 -Wformat-security \
-Wformat-overflow=2 -Wformat-truncation=2 \
-Walloca -Wvla \
-fno-omit-frame-pointer
INCLUDES = -I./include -I./src -I/usr/include
LDFLAGS = -L/usr/lib

# Debug flags
DEBUG_CFLAGS = -ggdb3 -O0 -DDEBUG -fno-omit-frame-pointer
DEBUG_LDFLAGS = -ggdb3

# Set flags based on build type
ifeq ($(DEBUG),1)
	CFLAGS += $(DEBUG_CFLAGS)
	LDFLAGS += $(DEBUG_LDFLAGS)
endif

# Profiling flags
PROF_CFLAGS = -pg -ggdb3 -fno-omit-frame-pointer
PROF_LDFLAGS = -pg

# Add profiling flags when PROFILE=1 is specified
ifeq ($(PROFILE),1)
	CFLAGS += $(PROF_CFLAGS)
	LDFLAGS += $(PROF_LDFLAGS)
endif

# Specific linking flags
MAIN_LDFLAGS = $(LDFLAGS) -static -pthread
TEST_LDFLAGS = $(LDFLAGS) -pthread -lcunit

# Source files
MAIN_SRC = $(SRC_DIR)/main.c
LIB_SRCS = $(filter-out $(MAIN_SRC),$(wildcard $(SRC_DIR)/*.c))
LIB_OBJS = $(LIB_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
MAIN_OBJ = $(MAIN_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_LIB_OBJS = $(filter-out $(MAIN_OBJ),$(LIB_OBJS))

# Final targets
MAIN_TARGET = $(BIN_DIR)/enssol
TEST_TARGET = $(BIN_DIR)/test_runner

# Default target
.PHONY: all clean clean-profile profile clean-profile debug test clean-test clean-logs
all: $(MAIN_TARGET)

# Directory creation
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Pattern rules for object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Main program build
$(MAIN_TARGET): $(MAIN_OBJ) $(LIB_OBJS) | $(BIN_DIR)
	$(CC) $^ $(MAIN_LDFLAGS) -o $@

# Test program build
$(TEST_TARGET): $(TEST_OBJS) $(TEST_LIB_OBJS) | $(BIN_DIR)
	$(CC) $^ $(TEST_LDFLAGS) -o $@

# Test target
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Debug target
debug: DEBUG=1
debug: all

# Clean targets
clean-all: clean clean-profile

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)

clean-profile:
	rm -f gmon.out profile.txt

# Profile target
profile: PROFILE=1
profile: clean-profile all
	./$(MAIN_TARGET)
	gprof $(MAIN_TARGET) gmon.out > profile.txt

# Add these cleanup targets

clean-test:
	rm -rf test_tmp
	rm -f test/*.log
	rm -rf test/shell_test/

clean-logs:
	rm -f var/log/*.log
	rm -f logs/*.log

clean: clean-all clean-test clean-logs

# Include dependency files (only if not cleaning)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clean-profile)
-include $(LIB_DEPS)
-include $(MAIN_DEPS)
-include $(TEST_DEPS)
endif
endif
