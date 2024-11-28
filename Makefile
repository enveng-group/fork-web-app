# Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
# SPDX-License-Identifier: 	AGPL-3.0-or-later

# POSIX-compliant Makefile for building and testing with musl

# For Trisquel use (CC = /usr/local/musl/bin/musl-clang-15 && LD=/usr/local/musl/bin/ld.musl-clang-15)
# For Debian use (CC = /usr/local/musl/bin/musl-clang && LD=/usr/local/musl/bin/ld.musl-clang)
CC = /usr/local/musl/bin/musl-clang
LD=/usr/local/musl/bin/ld.musl-clang
CFLAGS = -static -std=c2x -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=800 -O3 -Wall -Wextra -Werror -pedantic -Iinclude -Wshadow -Wconversion -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-declarations -Wredundant-decls -Wfloat-equal -Wundef -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wformat=2 -Wstrict-aliasing=2 -Wstrict-overflow=5 -Wc++-compat -Wnull-dereference -Wdouble-promotion -Wvla
LDFLAGS = -static -L/usr/local/musl/lib -lm -Wl,--no-undefined -Wl,--warn-common -Wl, -Wl
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests
INCLUDE_DIR = include
OBJ_FILES = $(BUILD_DIR)/main.o $(BUILD_DIR)/logger.o $(BUILD_DIR)/hello.o $(BUILD_DIR)/garbage_collector.o $(BUILD_DIR)/config_loader.o $(BUILD_DIR)/env_loader.o $(BUILD_DIR)/error_handler.o $(BUILD_DIR)/validator.o
TARGET = $(BIN_DIR)/my_program
TEST_TARGET = $(BIN_DIR)/test_runner

.PHONY: all clean test doc

all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJ_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LDFLAGS) -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Build and run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_DIR)/test_logger.o $(BUILD_DIR)/logger.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Generate documentation (man pages)
doc:
	groff -man docs/my_program.1 > $(BIN_DIR)/my_program.1.pdf
