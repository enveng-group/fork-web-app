# Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
# SPDX-License-Identifier: AGPL-3.0-or-later

CC = gcc
SRCDIR = src
TESTDIR = test
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include

# Common flags
COMMON_FLAGS = -std=c90 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 \
	-Wall -ansi -Wextra -pedantic -Werror -Wshadow \
	-Wconversion -Wstrict-prototypes -Wmissing-prototypes \
	-fanalyzer -fstack-protector-strong -fstack-check \
	-fdata-sections -ffunction-sections -fno-common -fstrict-aliasing \
	-Warray-bounds -Wstack-protector -Wformat=2 -Wformat-security \
	-Wformat-overflow=2 -Wformat-truncation=2 -Walloca -Wvla \
	-fno-omit-frame-pointer -finput-charset=iso-8859-1 \
	-fexec-charset=iso-8859-1 -fwide-exec-charset=iso-8859-1

# Production flags (static linking)
PROD_CFLAGS = $(COMMON_FLAGS) -static
PROD_LDFLAGS = -Wl,--gc-sections -static -lcrypt

# Test flags (dynamic linking)
TEST_CFLAGS = $(COMMON_FLAGS)
TEST_LDFLAGS = -Wl,--gc-sections
TEST_LIBS = -lcunit -lcrypt

# Source files
SRC = $(wildcard $(SRCDIR)/*.c)
TEST_SRC = $(wildcard $(TESTDIR)/*.c)

# Production source and object files
PROD_SRC = $(SRC)
PROD_OBJ = $(PROD_SRC:$(SRCDIR)/%.c=$(OBJDIR)/prod/%.o)

# Test source and object files (exclude main.c)
TEST_SRC_NO_MAIN = $(filter-out $(SRCDIR)/main.c,$(SRC))
TEST_OBJ = $(TEST_SRC_NO_MAIN:$(SRCDIR)/%.c=$(OBJDIR)/test/%.o) \
		   $(TEST_SRC:$(TESTDIR)/%.c=$(OBJDIR)/test/%.o)

# Targets
PROD_TARGET = $(BINDIR)/web_server
TEST_TARGET = $(BINDIR)/test_web_server

# Create necessary directories
$(shell mkdir -p $(OBJDIR)/prod $(OBJDIR)/test $(BINDIR))

# Release variables
VERSION ?= 0.0.1
RELEASE_NAME = web-app-$(VERSION)
RELEASE_DIR = $(BINDIR)/release/$(RELEASE_NAME)
RELEASE_TAR = $(BINDIR)/$(RELEASE_NAME).tar.gz

# Data files to include from project root and subdirectories
DATA_FILES = etc/auth.passwd var/db/ms1180.rec var/db/schema.desc var/db/scjv.rec var/db/w6946.rec var/log/audit.log

.PHONY: all clean test prod release release-prep clean-release

all: prod

prod: $(PROD_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Production build rules
$(PROD_TARGET): $(PROD_OBJ)
	$(CC) $^ -o $@ $(PROD_LDFLAGS)

$(OBJDIR)/prod/%.o: $(SRCDIR)/%.c
	$(CC) $(PROD_CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Test build rules
$(TEST_TARGET): $(TEST_OBJ)
	$(CC) $^ -o $@ $(TEST_LDFLAGS) $(TEST_LIBS)

$(OBJDIR)/test/%.o: $(SRCDIR)/%.c
	$(CC) $(TEST_CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/test/%.o: $(TESTDIR)/%.c
	$(CC) $(TEST_CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

release-prep: prod
	@echo "Preparing release $(RELEASE_NAME)..."
	@mkdir -p $(RELEASE_DIR)
	@# Copy binary
	@cp $(PROD_TARGET) $(RELEASE_DIR)/
	@# Copy data files
	@for file in $(DATA_FILES); do \
		dir=$$(dirname "$(RELEASE_DIR)/$$file"); \
		mkdir -p $$dir; \
		if [ -f $$file ]; then \
			cp $$file "$(RELEASE_DIR)/$$file"; \
		else \
			echo "Warning: $$file not found"; \
		fi \
	done
	@# Copy HTML files
	@if [ -d www ]; then \
		mkdir -p $(RELEASE_DIR)/www && \
		cp -r www/* $(RELEASE_DIR)/www/; \
	fi

release: release-prep
	@echo "Creating release archive..."
	@cd $(BINDIR)/release && tar -czf ../$(RELEASE_NAME).tar.gz $(RELEASE_NAME)
	@echo "Release archive created at $(RELEASE_TAR)"
	@# Optional: create checksum
	@cd $(BINDIR) && sha256sum $(RELEASE_NAME).tar.gz > $(RELEASE_NAME).tar.gz.sha256

clean-release:
	@echo "Cleaning release files..."
	@rm -rf $(BINDIR)/release
	@rm -f $(BINDIR)/*.tar.gz*

clean: clean-release
	rm -rf $(OBJDIR) $(BINDIR)
