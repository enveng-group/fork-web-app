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

.PHONY: all clean test prod release release-prep clean-release dist clean-dist

all: prod

prod: $(PROD_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Build directories
OBJDIRS = $(OBJDIR)/prod $(OBJDIR)/test
BINDIRS = $(BINDIR)
ALLDIRS = $(OBJDIRS) $(BINDIRS)

# Create required directories
$(OBJDIR)/prod:
	mkdir -p $@

$(OBJDIR)/test:
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

# Updated build rules
$(PROD_TARGET): $(ALLDIRS) $(PROD_OBJ)
	$(CC) $(PROD_OBJ) -o $@ $(PROD_LDFLAGS)

$(TEST_TARGET): $(ALLDIRS) $(TEST_OBJ)
	$(CC) $(TEST_OBJ) -o $@ $(TEST_LDFLAGS) $(TEST_LIBS)

# Updated object file rules
$(OBJDIR)/prod/%.o: $(SRCDIR)/%.c | $(OBJDIR)/prod
	$(CC) $(PROD_CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/test/%.o: $(SRCDIR)/%.c | $(OBJDIR)/test
	$(CC) $(TEST_CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/test/%.o: $(TESTDIR)/%.c | $(OBJDIR)/test
	$(CC) $(TEST_CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Release configuration
RELEASE_VERSION = 0.0.1
RELEASE_NAME = web-app-$(RELEASE_VERSION)
DISTDIR = dist
TMPDIR = /tmp/$(RELEASE_NAME)
PROD_TARGET = $(BINDIR)/web_server

# Release files and directories
RELEASE_DIRS = bin etc var/db var/log www

# Clean release target
dist: $(PROD_TARGET)
	# Create fresh temp directory
	rm -rf $(TMPDIR)
	mkdir -p $(TMPDIR)/bin
	mkdir -p $(TMPDIR)/etc
	mkdir -p $(TMPDIR)/var/db
	mkdir -p $(TMPDIR)/var/log
	mkdir -p $(TMPDIR)/www

	# Copy only runtime files
	cp $(PROD_TARGET) $(TMPDIR)/bin/
	cp etc/auth.passwd $(TMPDIR)/etc/
	cp var/db/*.rec $(TMPDIR)/var/db/
	cp var/db/schema.desc $(TMPDIR)/var/db/
	cp var/log/audit.log $(TMPDIR)/var/log/
	cp www/*.html $(TMPDIR)/www/

	# Create release archive
	mkdir -p $(DISTDIR)
	cd $(TMPDIR)/.. && tar czf $(CURDIR)/$(DISTDIR)/$(RELEASE_NAME).tar.gz $(RELEASE_NAME)

	# Cleanup
	rm -rf $(TMPDIR)

clean-dist:
	rm -rf $(DISTDIR)

release: clean-dist prod dist
	@echo "Release package created: $(DISTDIR)/$(RELEASE_NAME).tar.gz"
