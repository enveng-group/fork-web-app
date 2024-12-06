# Compiler and Flags
CC = /usr/bin/musl-gcc
CFLAGS = -std=c2x -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=800 -O3 -Wall -Wextra -Werror -pedantic -I/usr/include/x86_64-linux-musl -I$(PWD)/include -L/usr/lib/x86_64-linux-musl -nostdlib -static -fno-common -ffunction-sections -fdata-sections -march=x86-64 -Wshadow -Wconversion -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-declarations -Wredundant-decls -Wfloat-equal -Wundef -Wbad-function-cast -Wcast-qual -Wwrite-strings -Wformat=2 -Wstrict-aliasing=2 -Wstrict-overflow=5 -Wnull-dereference -Wdouble-promotion -Wvla -Wcast-align -Wpointer-arith -Wswitch-default -Wunreachable-code

# Output Directories
OBJDIR = obj
BINDIR = bin
INCDIR = include

# Source Files and Targets
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/web_app

# Create the necessary directories
$(OBJDIR) $(BINDIR):
	@mkdir -p $@

# Default target to build the program
all: $(TARGET)

# Link the object files to create the final binary
$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(OBJS) -o $@

# Compile source files into object files
$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Phony targets
.PHONY: all clean
