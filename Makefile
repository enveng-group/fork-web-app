# Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
# SPDX-License-Identifier: AGPL-3.0-or-later

# Detect environment and set compiler
ifeq ($(shell which gcc 2>/dev/null),)
	$(error gcc not found)
endif

# Check if we're on Alpine/musl system
LIBC_CHECK := $(shell ldd --version 2>&1 | grep -i musl)
ifneq ($(LIBC_CHECK),)
	# On Alpine/musl system, use gcc directly
	CC = gcc
else
	# On other systems, require musl-gcc
	ifeq ($(shell which musl-gcc 2>/dev/null),)
		$(error musl-gcc not found)
	endif
	CC = musl-gcc
endif

SRCDIR = src
TESTDIR = test
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include

# Add version info
MAJOR_VERSION = 0
MINOR_VERSION = 0
PATCH_VERSION = 2
VERSION = $(MAJOR_VERSION).$(MINOR_VERSION).$(PATCH_VERSION)

# Core language and standards flags
LANG_FLAGS = -std=c90 -ansi -pedantic \
	-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 \
	-D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE \
	-D_FORTIFY_SOURCE=2 -DVERSION=\"$(VERSION)\"

# Warning flags
WARN_FLAGS = -Wall -Wextra -Werror -Wshadow -Wconversion \
	-Wstrict-prototypes -Wmissing-prototypes -Warray-bounds \
	-Wformat=2 -Wformat-security -Wformat-overflow=2 \
	-Wformat-truncation=2 -Wvla -Wbad-function-cast \
	-Wstrict-aliasing=3 -Wnull-dereference -Wdouble-promotion \
	-Wfloat-equal -Wpointer-arith -Wwrite-strings -Wcast-align=strict \
	-Wcast-qual -Wredundant-decls -Wundef -Wmissing-include-dirs \
	-Winit-self -Wswitch-enum -Wmissing-declarations -Wsign-conversion \
	-Wmissing-noreturn -Wlogical-op -Wduplicated-cond -Wduplicated-branches \
	-Wunsafe-loop-optimizations -Wstack-usage=8192 -Wpadded \
	-Waggregate-return -Wattributes -Wbuiltin-macro-redefined \
	-Wdeprecated -Wdiv-by-zero -Wendif-labels -Wexpansion-to-defined \
	-Wformat-contains-nul -Wformat-extra-args -Wformat-zero-length \
	-Wmultichar -Wnested-externs -Wno-format-nonliteral \
	-Wno-unused-result -Woverflow -Wpointer-to-int-cast \
	-Wpragmas -Wredundant-decls -Wvariadic-macros -Wvla-larger-than=4096 \
	-Wwrite-strings -Wstack-protector

# Security flags
SEC_FLAGS = -fstack-protector-strong -fstack-clash-protection \
	-fstack-check -fPIE -fstack-protector-all \
	-fno-delete-null-pointer-checks -ftrivial-auto-var-init=zero \
	-fno-allow-store-data-races -fwrapv -fno-strict-volatile-bitfields \
	-finput-charset=iso-8859-1 -fexec-charset=iso-8859-1 \
	-fwide-exec-charset=iso-8859-1 -fstack-protector

# CPU-specific optimization flags
ARCH_FLAGS = -march=native -mtune=native \
	-msse4.2 -mavx2 -mfma -mavx512f \
	-mpclmul -maes -msha \
	-mprefer-vector-width=512

# Cache optimization for Graviton2
CACHE_FLAGS = -falign-functions=64 \
	-falign-jumps=32 \
	-falign-loops=32 \
	-falign-labels=32

# Network performance flags
NET_FLAGS = -fno-schedule-insns \
	-fno-schedule-insns2 \
	-fomit-frame-pointer \
	-fno-stack-check

# Update ARCH_FLAGS for t4g
ARCH_FLAGS = $(CACHE_FLAGS) $(NET_FLAGS)

# Enhanced optimization flags
OPT_FLAGS = -O3 -fdata-sections -ffunction-sections \
	-fno-common -fstrict-aliasing -fno-strict-overflow \
	-fno-plt -fPIC -fno-semantic-interposition \
	-flto=auto -fno-fat-lto-objects \
	-fdevirtualize-at-ltrans -fipa-pta \
	-floop-nest-optimize -floop-parallelize-all \
	-ftree-parallelize-loops=4 -ftree-vectorize \
	-funroll-loops -ffast-math -fmerge-all-constants \
	-fmodulo-sched -fmodulo-sched-allow-regmoves \
	-fgcse-sm -fgcse-las -fipa-cp-clone \
	-fira-loop-pressure -fivopts -flifetime-dse=2 \
	-flive-range-shrinkage -fmath-errno -fpeephole2 \
	-freciprocal-math -fsched-pressure -fsched-spec-load \
	-fsched2-use-superblocks -ftree-loop-distribution \
	-ftree-loop-im -ftree-loop-ivcanon -ftree-vectorize \
	-fuse-linker-plugin -fwhole-program \
	-falign-functions=32 -falign-jumps=32 \
	-falign-loops=32 -falign-labels=32 \
	-fcaller-saves -fcrossjumping -fcse-follow-jumps \
	-fcse-skip-blocks -fdelete-null-pointer-checks \
	-fdevirtualize -fexpensive-optimizations \
	-ffinite-loops -fgcse-after-reload -fhoist-adjacent-loads \
	-findirect-inlining -finline-functions \
	-finline-small-functions -fipa-bit-cp -fipa-icf \
	-fipa-icf-functions -fipa-icf-variables -fipa-profile \
	-fipa-pure-const -fipa-ra -fipa-reference \
	-fipa-reference-addressable -fipa-sra -fira-hoist-pressure \
	-fisolate-erroneous-paths-dereference -flra-remat \
	-foptimize-sibling-calls -fpartial-inlining \
	-fpeel-loops -fpredictive-commoning \
	-free -frename-registers -freorder-blocks \
	-freorder-blocks-algorithm=stc -freorder-functions \
	-frerun-cse-after-loop -fschedule-fusion -freschedule-modulo-scheduled-loops \
	-fsplit-ivs-in-unroller -fssa-phiopt -fstore-merging \
	-fstrict-aliasing -fthread-jumps -ftree-bit-ccp \
	-ftree-builtin-call-dce -ftree-copy-prop -ftree-dce \
	-ftree-dominator-opts -ftree-dse -ftree-forwprop \
	-ftree-fre -ftree-loop-if-convert -ftree-loop-im \
	-ftree-loop-ivcanon -ftree-loop-optimize \
	-ftree-parallelize-loops=4 -ftree-pre -ftree-pta \
	-ftree-reassoc -ftree-sink -ftree-slsr -ftree-sra \
	-ftree-switch-conversion -ftree-tail-merge \
	-ftree-ter -ftree-vrp -funit-at-a-time -funswitch-loops \
	-fvariable-expansion-in-unroller -fvect-cost-model=dynamic \
	-fversion-loops-for-strides $(ARCH_FLAGS)

# Combine all CFLAGS
PROD_CFLAGS = $(LANG_FLAGS) $(WARN_FLAGS) $(SEC_FLAGS) $(OPT_FLAGS) -static

# Enhanced linker flags for performance
PROD_LDFLAGS = -static -Wl,--gc-sections,-O3,--sort-common \
	-Wl,--build-id=none,-z,relro,-z,now,-z,noexecstack \
	-Wl,--sort-section=alignment,--strip-all,--warn-common \
	-Wl,--no-undefined,--no-allow-shlib-undefined \
	-Wl,--no-copy-dt-needed-entries,--as-needed \
	-Wl,-z,separate-code,-z,text,-z,defs \
	-Wl,-z,relro,-z,global \
	-Wl,-z,noexecstack,-z,nocombreloc \
	-Wl,--eh-frame-hdr,--exclude-libs,ALL \
	-Wl,--sort-section=alignment \
	-Wl,--relax \
	-Wl,-O3 \
	-lpthread -lcrypt -lrt -ldl

# Test & Debug configuration flags
TEST_BASIC = -std=c90 -ansi -pedantic \
	-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 \
	-Wall -Wextra -Werror -O0

# Debug symbol flags for GDB
TEST_DEBUG = -g3 -ggdb3 -gdwarf-4 \
	-fno-eliminate-unused-debug-types \
	-fno-omit-frame-pointer \
	-fno-inline

# Coverage flags
TEST_COVERAGE = -fprofile-arcs -ftest-coverage \
	-fprofile-generate -fvpt -funroll-loops

# Refined analysis flags
TEST_ANALYSIS = -fanalyzer -fno-omit-frame-pointer \
	-fstack-check -fstack-protector-all -ftrapv

# Combine all test CFLAGS
TEST_CFLAGS = $(TEST_BASIC) $(TEST_DEBUG) \
	$(TEST_COVERAGE) $(filter-out -O3,$(TEST_ANALYSIS))

# Test libraries and linker flags
TEST_LIBS = -lgcov -lcunit -lcrypt -lpthread -lrt
TEST_LDFLAGS = -Wl,--warn-common \
	-Wl,--no-undefined \
	-Wl,--gc-sections \
	-Wl,--build-id \
	-Wl,--eh-frame-hdr \
	$(TEST_LIBS)

# Source files
SRC = $(wildcard $(SRCDIR)/*.c)
TEST_SRC = $(wildcard $(TESTDIR)/*.c)
PROD_SRC = $(SRC)

# Add header files tracking
HDRS = $(wildcard $(INCLUDEDIR)/*.h)

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

.PHONY: all prod test dist clean-dist release clean check uninstall debug help distclean

all: prod

prod: $(PROD_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

check: $(TEST_TARGET)
	./$(TEST_TARGET) --no-coverage

debug: $(TEST_TARGET)
	gdb $(TEST_TARGET)

help:
	@echo "Available targets:"
	@echo "  all        - Build production binary (default)"
	@echo "  prod       - Build production binary"
	@echo "  test       - Build and run tests with coverage"
	@echo "  check      - Build and run tests without coverage"
	@echo "  debug      - Build tests and launch GDB"
	@echo "  clean      - Remove build artifacts"
	@echo "  install    - Install the application"
	@echo "  uninstall  - Uninstall the application"
	@echo "  dist       - Create distribution package"
	@echo "  release    - Create versioned release package"
	@echo "  help       - Show this help message"

distclean: clean clean-dist
	rm -f $(DEPFILES)

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

# Add header dependencies
$(PROD_OBJ): $(HDRS)
$(TEST_OBJ): $(HDRS)

# Release configuration
VERSION ?= 0.0.1
RELEASE_NAME = web-app-$(VERSION)
DISTDIR = dist
TMPDIR = /tmp/$(RELEASE_NAME)

# Define required directories and files
WEB_FILES = www/audit.html www/crud_profile.html www/crud_scjv.html \
	www/dashboard.html www/index.html www/ms1180.html \
	www/profile.html www/scjv.html www/w6946.html

REC_FILES = var/records/ms1180.rec var/records/obligation_number.txt \
	var/records/schema.desc var/records/scjv.rec var/records/w6946.rec

LOG_FILES = var/log/audit.log

# Updated dist target
dist: $(PROD_TARGET)
	rm -rf $(TMPDIR)
	# Create directory structure
	mkdir -p $(TMPDIR)
	mkdir -p $(TMPDIR)/etc
	mkdir -p $(TMPDIR)/var/log
	mkdir -p $(TMPDIR)/var/records
	mkdir -p $(TMPDIR)/www
	# Copy executable
	cp $(PROD_TARGET) $(TMPDIR)/web_server
	# Copy auth file
	cp etc/auth.passwd $(TMPDIR)/etc/
	# Copy record files
	cp var/records/ms1180.rec $(TMPDIR)/var/records/
	cp var/records/obligation_number.txt $(TMPDIR)/var/records/
	cp var/records/schema.desc $(TMPDIR)/var/records/
	cp var/records/scjv.rec $(TMPDIR)/var/records/
	cp var/records/w6946.rec $(TMPDIR)/var/records/
	# Copy log files
	cp var/log/audit.log $(TMPDIR)/var/log/
	# Copy web files
	cp www/audit.html $(TMPDIR)/www/
	cp www/crud_profile.html $(TMPDIR)/www/
	cp www/crud_scjv.html $(TMPDIR)/www/
	cp www/dashboard.html $(TMPDIR)/www/
	cp www/index.html $(TMPDIR)/www/
	cp www/ms1180.html $(TMPDIR)/www/
	cp www/profile.html $(TMPDIR)/www/
	cp www/scjv.html $(TMPDIR)/www/
	cp www/w6946.html $(TMPDIR)/www/
	$(MAKE) t4g-permissions
	# Create tarball
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

# Add install target
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

.PHONY: install
install: prod
	install -d $(DESTDIR)$(bindir)
	install -m 755 $(PROD_TARGET) $(DESTDIR)$(bindir)

# Add uninstall target
.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(bindir)/$(notdir $(PROD_TARGET))

# Add profile-guided optimization targets
.PHONY: pgo-generate pgo-use

pgo-generate: PROD_CFLAGS += -fprofile-generate
pgo-generate: clean prod
	./$(PROD_TARGET) # Run your program with typical workload

pgo-use: PROD_CFLAGS += -fprofile-use -fprofile-correction
pgo-use: clean prod

# Optimized production build
opt: pgo-generate pgo-use

# Add new target for t4g builds
.PHONY: t4g-release

t4g-release: CC = aarch64-linux-musl-gcc
t4g-release: clean-dist prod dist
	@echo "T4g optimized release package created: $(DISTDIR)/$(RELEASE_NAME).tar.gz"

# Update permissions for t4g deployment
t4g-permissions:
	chmod 755 $(TMPDIR)
	chmod 755 $(TMPDIR)/etc
	chmod 755 $(TMPDIR)/var
	chmod 755 $(TMPDIR)/var/log
	chmod 755 $(TMPDIR)/var/records
	chmod 755 $(TMPDIR)/www
	chmod 644 $(TMPDIR)/etc/auth.passwd
	chmod 644 $(TMPDIR)/var/records/*
	chmod 644 $(TMPDIR)/var/log/*
	chmod 644 $(TMPDIR)/www/*.html
	chmod 755 $(TMPDIR)/web_server
