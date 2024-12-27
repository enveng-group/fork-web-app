# Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
# SPDX-License-Identifier: AGPL-3.0-or-later

CC = gcc
SRCDIR = src
TESTDIR = test
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include

# Common flags combined on single lines to avoid continuation issues
COMMON_FLAGS = -std=c90 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -Wall -ansi -Wextra -pedantic -Werror -Wshadow -Wconversion -Wstrict-prototypes -Wmissing-prototypes -Warray-bounds -Wformat=2 -Wformat-security -Wformat-overflow=2 -Wformat-truncation=2 -Wvla -Wbad-function-cast -Wstrict-aliasing=2 -Wnull-dereference -Wdouble-promotion -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wcast-align -Wcast-qual -Wredundant-decls -Wundef -Wmissing-include-dirs -Winit-self -Wswitch-enum -Wmissing-declarations -Wsign-conversion -fstack-protector-strong -fstack-check -fPIE -fstack-protector-all -fdata-sections -ffunction-sections -fno-common -fstrict-aliasing -fno-strict-overflow -fanalyzer -fno-omit-frame-pointer -finput-charset=iso-8859-1 -fexec-charset=iso-8859-1 -fwide-exec-charset=iso-8859-1

PROD_CFLAGS = $(COMMON_FLAGS) -static -O3
PROD_LDFLAGS = -static -Wl,--gc-sections -Wl,-z,relro,-z,now -Wl,-z,noexecstack -Wl,--sort-section=alignment -Wl,--warn-common -Wl,--no-undefined -Wl,--as-needed -Wl,--strip-all -Wl,--build-id=none -lpthread -lcrypt

TEST_CFLAGS = $(COMMON_FLAGS) -g3 -O0 -fprofile-arcs -ftest-coverage -fstack-protector-all
TEST_LIBS = -lgcov -lcunit -lcrypt -lpthread -lrt -ldl
TEST_LDFLAGS = -Wl,--warn-common -Wl,--no-undefined -Wl,--fatal-warnings \
			   -Wl,--gc-sections -Wl,-export-dynamic -Wl,--build-id \
			   -Wl,--eh-frame-hdr -Wl,-Map=test.map -Wl,--trace -Wl,--verbose \
			   $(TEST_LIBS)

# Source files
SRC = $(wildcard $(SRCDIR)/*.c)
TEST_SRC = $(wildcard $(TESTDIR)/*.c)
PROD_SRC = $(SRC)

# Object files
PROD_OBJ = $(PROD_SRC:$(SRCDIR)/%.c=$(OBJDIR)/prod/%.o)
TEST_SRC_NO_MAIN = $(filter-out $(SRCDIR)/main.c,$(SRC))
TEST_OBJ = $(TEST_SRC_NO_MAIN:$(SRCDIR)/%.c=$(OBJDIR)/test/%.o) \
		   $(TEST_SRC:$(TESTDIR)/%.c=$(OBJDIR)/test/%.o)

# Dependency files
DEPFILES := $(PROD_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

PROD_TARGET = $(BINDIR)/web_server
TEST_TARGET = $(BINDIR)/test_web_server

# Build directories
OBJDIRS = $(OBJDIR)/prod $(OBJDIR)/test
BINDIRS = $(BINDIR)
ALLDIRS = $(OBJDIRS) $(BINDIRS)

.PHONY: all prod test dist clean-dist release clean

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

$(TEST_OBJ): | $(OBJDIR)/test

# Ensure the production object directory exists before building prod objects
$(PROD_OBJ): | $(OBJDIR)/prod

# Release configuration
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

# Include generated dependency files
-include $(PROD_OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)

# Generate dependencies
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	@set -e; rm -f $@; \
	$(CC) -MM -I$(INCLUDEDIR) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(OBJDIR)/test_%.d: $(TESTDIR)/%.c
	@set -e; rm -f $@; \
	$(CC) -MM -I$(INCLUDEDIR) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/test_\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# Ensure dependency files can be generated in obj/prod
$(OBJDIR)/prod/%.d: $(SRCDIR)/%.c | $(OBJDIR)/prod
	@set -e; rm -f $@; \
	$(CC) -MM -I$(INCLUDEDIR) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/prod/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# Ensure dependency files can be generated in obj/test
$(OBJDIR)/test/%.d: $(TESTDIR)/%.c | $(OBJDIR)/test
	@set -e; rm -f $@; \
	$(CC) -MM -I$(INCLUDEDIR) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)/test/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# Remove redundant includes if already covered by -include $(DEPFILES)
# -include $(PROD_OBJS:.o=.d)
# -include $(TEST_OBJS:.o=.d)
