# Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
# SPDX-License-Identifier: AGPL-3.0-or-later

CC = gcc
SRCDIR = src
TESTDIR = test
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include

# Common flags
COMMON_FLAGS = -std=c90 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -Wall -ansi \
	-Wextra -pedantic -Werror -Wshadow -Wconversion -Wstrict-prototypes \
	-Wmissing-prototypes -fanalyzer -fstack-protector-strong -fstack-check \
	-fdata-sections -ffunction-sections -fno-common -fstrict-aliasing \
	-Warray-bounds -Wstack-protector -Wformat=2 -Wformat-security \
	-Wformat-overflow=2 -Wformat-truncation=2 -Walloca -Wvla \
	-fno-omit-frame-pointer

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

.PHONY: all clean test prod

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

clean:
	rm -rf $(OBJDIR) $(BINDIR)
