# Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
# SPDX-License-Identifier: AGPL-3.0-or-later

CC = gcc
SRCDIR = src
TESTDIR = test
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include

COMMON_FLAGS = -std=c90 -O3 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 \
	-Wall -ansi -Wextra -pedantic -Werror -Wshadow \
	-Wconversion -Wstrict-prototypes -Wmissing-prototypes \
	-fanalyzer -fstack-protector-strong -fstack-check \
	-fdata-sections -ffunction-sections -fno-common -fstrict-aliasing \
	-Warray-bounds -Wstack-protector -Wformat=2 -Wformat-security \
	-Wformat-overflow=2 -Wformat-truncation=2 -Walloca -Wvla \
	-fno-omit-frame-pointer -finput-charset=iso-8859-1 \
	-fexec-charset=iso-8859-1 -fwide-exec-charset=iso-8859-1 \
	-Wbad-function-cast -Wstrict-aliasing=2 \
	-Wnull-dereference -Wdouble-promotion -Wfloat-equal \
	-Wpointer-arith -Wwrite-strings -Wunreachable-code \
	-Wcast-align -Wswitch-default

PROD_CFLAGS = $(COMMON_FLAGS) -static
PROD_LDFLAGS = -Wl,--gc-sections -static -lcrypt
TEST_CFLAGS = $(COMMON_FLAGS)
TEST_LDFLAGS = -Wl,--gc-sections
TEST_LIBS = -lcunit -lcrypt

SRC = $(wildcard $(SRCDIR)/*.c)
TEST_SRC = $(wildcard $(TESTDIR)/*.c)
PROD_SRC = $(SRC)
PROD_OBJ = $(PROD_SRC:$(SRCDIR)/%.c=$(OBJDIR)/prod/%.o)

TEST_SRC_NO_MAIN = $(filter-out $(SRCDIR)/main.c,$(SRC))
TEST_OBJ = $(TEST_SRC_NO_MAIN:$(SRCDIR)/%.c=$(OBJDIR)/test/%.o) \
		   $(TEST_SRC:$(TESTDIR)/%.c=$(OBJDIR)/test/%.o)

PROD_TARGET = $(BINDIR)/web_server
TEST_TARGET = $(BINDIR)/test_web_server

# Build directories (no duplicate mkdir calls)
OBJDIRS = $(OBJDIR)/prod $(OBJDIR)/test
BINDIRS = $(BINDIR)
ALLDIRS = $(OBJDIRS) $(BINDIRS)

DEPFILES := $(PROD_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

all: prod

prod: $(PROD_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(OBJDIR)/prod:
	mkdir -p $@

$(OBJDIR)/test:
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

$(PROD_TARGET): $(ALLDIRS) $(PROD_OBJ)
	$(CC) $(PROD_OBJ) -o $@ $(PROD_LDFLAGS)

$(TEST_TARGET): $(ALLDIRS) $(TEST_OBJ)
	$(CC) $(TEST_OBJ) -o $@ $(TEST_LDFLAGS) $(TEST_LIBS)

-include $(DEPFILES)

$(OBJDIR)/prod/%.o: $(SRCDIR)/%.c
	$(CC) -MMD -MP $(PROD_CFLAGS) -c $< -o $@

$(OBJDIR)/test/%.o: $(SRCDIR)/%.c
	$(CC) -MMD -MP $(TEST_CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

$(OBJDIR)/test/%.o: $(TESTDIR)/%.c
	$(CC) -MMD -MP $(TEST_CFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Add pattern rule for test objects from source files
$(TEST_OBJ): | $(OBJDIR)/test

# Fix test source patterns
TEST_SRC = $(wildcard $(TESTDIR)/*.c)
TEST_SRC_NO_MAIN = $(filter-out $(SRCDIR)/main.c,$(SRC))
TEST_OBJ = $(TEST_SRC_NO_MAIN:$(SRCDIR)/%.c=$(OBJDIR)/test/%.o) \
		   $(TEST_SRC:$(TESTDIR)/%.c=$(OBJDIR)/test/%.o)

# Consolidated release config
VERSION ?= 0.0.1
RELEASE_NAME = web-app-$(VERSION)
DISTDIR = dist
TMPDIR = /tmp/$(RELEASE_NAME)

dist: $(PROD_TARGET)
	rm -rf $(TMPDIR)
	mkdir -p $(TMPDIR)/etc
	mkdir -p $(TMPDIR)/var/records
	mkdir -p $(TMPDIR)/var/log
	mkdir -p $(TMPDIR)/www
	cp $(PROD_TARGET) $(TMPDIR)/
	cp etc/auth.passwd $(TMPDIR)/etc/
	cp var/records/*.rec $(TMPDIR)/var/records/
	cp var/records/schema.desc $(TMPDIR)/var/records/
	cp var/log/audit.log $(TMPDIR)/var/log/
	cp www/*.html $(TMPDIR)/www/
	mkdir -p $(DISTDIR)
	cd $(TMPDIR)/.. && tar czf $(CURDIR)/$(DISTDIR)/$(RELEASE_NAME).tar.gz $(RELEASE_NAME)
	rm -rf $(TMPDIR)

clean-dist:
	rm -rf $(DISTDIR)

release: clean-dist prod dist
	@echo "Release package created: $(DISTDIR)/$(RELEASE_NAME).tar.gz"

clean:
	rm -rf $(OBJDIR) $(BINDIR)/*.o $(BINDIR)/web_server $(BINDIR)/test_web_server

.PHONY: all prod test dist clean-dist release clean
