# **Copilot Instructions for EnvEng Web Application Development**

These instructions ensure Copilot's suggestions align with our project's technical standards and development environment.

---

## **General Guidelines**
- Adhere to **ISO/IEC 9899:1990 (C90 Standard)**, ** POSIX.1 (ISO/IEC 9945-1:1996 - IEEE Standard 1003.1)**, and **X/Open-500** compliance in all code suggestions.
- Ensure code is **portable**, **cross-platform**, and **cross-architecture**:
    - Prioritize compatibility with `musl libc`, and `gcc` (for Alpine) `musl-gcc` (for Trisquel).
    - Avoid features requiring `glibc` or non-portable platform-specific APIs.
- Write code optimized for lightweight environments (e.g. `musl`).
- Data oriented design is preferred over object oriented design.
- Format C Code following GNU Coding Standards.
- Do not use C++ comments (`//`) in C code.
- Do not use undeclared identifier.
- Generate code that does not allow variable declarations in for loop initializers
- Always define variables at the beginning of the block.
- Always define functions before they are called.
- Always define reference variables with the `&` operator.
- Always define pointers with the `*` operator.
- Always define arrays with the `[]` operator.
- Never generate code that has undefined reference behaviour.
- Never redefine a variable in the same scope.
- Avoid generating code that are conflicting types for the same variable.
- Avoid previous declarations of variables.
- Code must compile with gcc using: `-std=c90 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -Wall -ansi -Wextra -pedantic -Werror -Wshadow -Wconversion -Wstrict-prototypes -Wmissing-prototypes -fanalyzer -fstack-protector-strong -fstack-check -fdata-sections -ffunction-sections -fno-common -fstrict-aliasing -Warray-bounds -Wstack-protector -Wformat=2 -Wformat-security -Wformat-overflow=2 -Wformat-truncation=2 -Walloca -Wvla -fno-omit-frame-pointer`
- Code Safety Requirements:
    - No VLAs (enforced by -Wvla)
    - No array bounds violations (enforced by -Warray-bounds)
    - No stack buffer overflows (enforced by -fstack-protector-strong -fstack-check)
    - No undefined behavior
    - No memory leaks
    - No floating-point issues
- Memory Safety:

```c
/* Always initialize pointers */
int *ptr = NULL;

/* Check malloc results */
ptr = malloc(size);
if (ptr == NULL) {
    /* Handle error */
    return -1;
}
```
- Array Safety:

```c
/* Define array size as constant */
#define ARRAY_SIZE 100

/* Use size_t for array indices */
size_t i;
int array[ARRAY_SIZE];

/* Array bounds checking */
for (i = 0; i < ARRAY_SIZE; i++) {
    array[i] = 0;
}
```

- Function Prototypes:

```c
/* Complete prototype with parameter names */
int function_name(int parameter_one, char *parameter_two);

/* Implementation matches exactly */
int
function_name(int parameter_one, char *parameter_two)
{
    /* Function body */
    return 0;
}
```

- Variable Declaration:

```c
void function(void)
{
    /* All declarations at start */
    int i;
    char *ptr = NULL;
    const char *str = "example";

    /* Code follows */
    i = 0;
}
```

- String Handling:

```c
/* Safe string operations */
char buffer[MAX_SIZE];
size_t len;

if (snprintf(buffer, sizeof(buffer), "%s", input) >= sizeof(buffer)) {
    /* Handle truncation */
    return -1;
}
```

- Format String Safety

```c
/* Use format string prototypes */
int print_value(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/* Check format string usage */
printf("%s", str);  /* correct */
printf(str);        /* wrong - format string vulnerability */
```

- Thread Safety

```c
/* Thread-local storage when needed */
static __thread int thread_local_var;

/* Proper mutex usage */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_lock(&mutex);
/* Critical section */
pthread_mutex_unlock(&mutex);
```

- Error Handling

```c
/* Check system calls */
if (write(fd, buf, len) < 0) {
    /* Handle error with errno */
    return -1;
}

/* Check library functions */
if (pthread_mutex_lock(&mutex) != 0) {
    /* Handle error */
    return -1;
}
```

- Type Safety

```c
/* Use explicit types */
uint32_t unsigned_val;
int32_t signed_val;

/* Type conversion with checks */
if (val > INT_MAX || val < INT_MIN) {
    /* Handle overflow */
    return -1;
}
```

---

## **Code Structure**
- Use only ANSI C headers
- Include POSIX headers with proper guards
- Order: system headers, then local headers
- Use `snake_case` for file names, e.g., `data_manager.c`, `user_auth.h`.

## **FOrmatting Style**
- K&R/GNU style bracing
- Full prototype declarations
- No inline functions (C90 restriction)
- Return type on separate line

## **Variable Declarations**
- All declarations at block start
- One variable per declaration
- No VLAs (C90 restriction)
- No mixed declarations/code

### **Naming Conventions**
- **Variables**: Use `snake_case` (e.g., `user_id`, `config_path`).
- **Functions**: Use `camelCase` (e.g., `processData`, `validateUser`).
- **Constants**: Use `UPPER_CASE` (e.g., `MAX_BUFFER_SIZE`, `DEFAULT_TIMEOUT`).

---

## **Coding Practices**
### **Data Handling**
- Keep data definitions separate from logic:
    - Define data structures in header files (`.h`).
    - Implement logic in source files (`.c`).
- Use immutable data structures whenever applicable for safety and clarity.

## Optimised Directory Structure
```plaintext
/			# . (root directory)
├── bin/                # Essential user binaries
│   ├── sh              # POSIX-compliant shell
│   ├── ls              # List directory contents
│   ├── cp              # Copy files
│   ├── mv              # Move/rename files
│   ├── rm              # Remove files
│   ├── mkdir           # Create directories
│   └── ...             # Other essential commands
├── sbin/               # Essential system binaries
│   ├── init            # Init system
│   ├── fsck            # Filesystem check
│   ├── mount           # Mount filesystems
│   ├── umount          # Unmount filesystems
│   └── ...             # Other system utilities
├── lib/                # Shared libraries for essential binaries
│   ├── libc.so         # musl libc
│   ├── libpthread.so   # POSIX threads
│   ├── libm.so         # Math library
│   └── ...             # Other essential shared libraries
├── etc/                # System configuration files
│   ├── passwd          # User account information
│   ├── group           # Group account information
│   ├── fstab           # Filesystem table
│   ├── hosts           # Hostname resolution
│   └── ...             # Other configuration files
├── dev/                # Device files
│   ├── null            # Null device
│   ├── zero            # Zero device
│   ├── tty             # Terminal device
│   ├── random          # Random number generator
│   ├── sda             # First disk device
│   └── ...             # Other device nodes
├── tmp/                # Temporary files
├── var/                # Variable data
│   ├── log/            # Log files
│   │   ├── syslog      # System log
│   │   ├── auth.log    # Authentication log
│   │   └── ...         # Other log files
│   ├── spool/          # Spool directories
│   └── run/            # Runtime data
├── usr/                # Secondary hierarchy for user programs
│   ├── bin/            # Non-essential user binaries
│   ├── lib/            # Shared libraries for `/usr/bin`
│   ├── include/        # Header files
│   ├── share/          # Architecture-independent files
│   │   ├── man/        # Manual pages
│   │   └── locale/     # Localization files
│   └── src/            # Source code (optional)
├── home/               # User home directories
│   ├── user1/          # Example user
│   └── user2/          # Another user
├── opt/                # Optional software packages
├── mnt/                # Mount points for temporary filesystems
│   ├── usb/            # USB devices
│   ├── cdrom/          # CD-ROM
│   └── ...             # Other mount points
├── proc/               # Virtual filesystem for processes
├── sys/                # Virtual filesystem for system information
└── boot/               # Bootloader files (if applicable)
    ├── kernel          # Kernel image
    ├── initrd          # Initial RAM disk
    └── ...             # Other boot files
```

## **1. Essential Components of a POSIX-Compliant Filesystem**
### **Filesystem Hierarchy**
Your filesystem must support the essential directories and their roles as outlined by POSIX and the Filesystem Hierarchy Standard (FHS). Key directories include:

#### **Root Directory (`/`)**
The root directory is the starting point of the filesystem and contains essential subdirectories:
- `/bin`: Essential user binaries (e.g., `ls`, `cp`, `sh`).
- `/sbin`: Essential system binaries (e.g., `init`, `fsck`).
- `/lib`: Shared libraries needed by binaries in `/bin` and `/sbin`.
- `/etc`: System configuration files.
- `/dev`: Device files (e.g., `tty`, `null`, `random`).
- `/tmp`: Temporary files (cleared on reboot).
- `/var`: Variable data (e.g., logs, spools, runtime data).
- `/usr`: Secondary hierarchy for user applications and libraries.
  - `/usr/bin`: Non-essential user binaries.
  - `/usr/lib`: Libraries for `/usr/bin`.
  - `/usr/include`: Header files for development.
  - `/usr/share`: Architecture-independent files (e.g., man pages, locale files).

#### **Optional Directories**
- `/home`: User home directories.
- `/opt`: Optional application software packages.
- `/mnt` or `/media`: Mount points for external filesystems.
- `/proc`: Virtual filesystem for process and system information.
- `/sys`: Virtual filesystem for system hardware and kernel information.

---

## **2. Key Filesystem Components**
To build a fully functional POSIX filesystem, you need to consider the following:

### **a. Kernel Interface**
- Implement system calls for POSIX-compliant operations like file handling (`open`, `read`, `write`, `close`), process control (`fork`, `exec`, `wait`), and inter-process communication (IPC).
- Ensure compatibility with the Virtual File System (VFS) layer if using a Linux kernel.

### **b. Filesystem Implementation**
- Support for at least one filesystem type (e.g., ext4, XFS, or a custom implementation).
- Implement file metadata handling, including permissions, ownership, timestamps, and symbolic/hard links.

### **c. Device Files**
- Provide `/dev` with character and block device nodes for hardware interaction.
- Use `mknod` to create device files with proper major and minor numbers.

### **d. Shared Libraries**
- Provide essential libraries in `/lib` and `/usr/lib`, including:
  - **musl libc**: A lightweight, POSIX-compliant standard C library.
  - **libm**: Math library.
  - **libpthread**: POSIX threads library.

---

## **3. Tools and Utilities**
### **a. Essential Utilities**
Include the POSIX-mandated utilities, such as:
- File management: `ls`, `cp`, `mv`, `rm`, `mkdir`.
- Shell utilities: `sh`, `echo`, `test`, `kill`.
- Text processing: `cat`, `grep`, `sed`, `awk`.
- System management: `ps`, `top`, `df`, `du`, `mount`, `umount`.

### **b. Development Tools**
- A compliant C compiler (e.g., `musl-gcc` or a custom ANSI C compiler).
- Tools for building and debugging, such as `make` and `gdb`.

### **c. Init System**
- Implement a minimal init system (e.g., `init`) to handle system startup and service management.

---

## **4. POSIX Compliance Checklist**
To ensure POSIX compliance:
- **File and Directory Structure**: Verify the existence of mandatory directories.
- **System Calls**: Implement all required POSIX system calls.
- **Threads and Concurrency**: Provide support for POSIX threads (pthreads).
- **Signals and IPC**: Implement signal handling and IPC mechanisms (pipes, message queues, semaphores, shared memory).
- **Filesystem Features**: Support file permissions, ownership, and links.

---

## **5. Standards Compliance**
### **a. C89/C90 Compliance**
- Use `musl libc` or a custom ANSI C library implementation.
- Ensure strict adherence to C89/C90 for the entire codebase.

### **b. X/Open Portability**
- Implement the X/Open Portability Guide (XPG) features, including extended file attributes, locale support, and terminal interfaces.

---

## **6. Build and Deployment**
### **a. Static Build**
- Ensure all binaries are statically linked to avoid runtime dependencies.

### **b. Cross-Platform Compatibility**
- Use tools like `musl-gcc` to create cross-platform binaries.
- Test on multiple architectures (e.g., x86, ARM).

### **c. Build Tools**
- Use a portable build system (e.g., `make`) to compile the filesystem and utilities.

---

## **7. Documentation and Testing**
### **a. Documentation**
- Provide detailed documentation for the filesystem structure and utilities in Markdown.
- Include man pages for all commands and libraries.

### **b. Testing**
- Write unit tests for all libraries and utilities.
- Use test suites like `POSIX Test Suite (PTS)` to validate compliance.


### **Error Handling**
- Use errno for error reporting
- Check all system call returns
- No exceptions (C90)
- No goto (except error cleanup)
- Always check return values of system calls and library functions, including:
    - Memory allocation (`malloc`, `calloc`).
    - File I/O (`fopen`, `fread`, `fwrite`).
    - Threading functions (`pthread_mutex_lock`, `pthread_create`).
- Use consistent error codes (`errno`) and logging to diagnose failures.

### **Memory Management**
- Prevent memory leaks by always pairing:
    - **Allocation** (`malloc`, `calloc`) with **Deallocation** (`free`).
- Avoid using dynamic memory where possible in performance-critical paths.
- Use stack allocation for predictable performance and reduced overhead.

### **Performance**
- Optimize for **CPU cache efficiency**:
    - Use arrays instead of linked lists for sequential data access.
    - Minimize pointer indirection and fragmentation.
- Enable compiler optimizations with appropriate flags (e.g., `-ffunction-sections`, `-fdata-sections`).
- Prefer static linking (`-static`) to ensure binaries are self-contained and lightweight.

### **Thread Safety**
- Use POSIX threading primitives:
    - Mutexes (`pthread_mutex_t`) for synchronized access to shared resources.
    - Condition variables (`pthread_cond_t`) for thread signaling.
- Avoid global variables or use them sparingly with proper synchronization.
- Ensure all functions are reentrant if they may be called in multi-threaded contexts.

---

## **Environment-Specific Considerations**
- **Compiler**: Target **x86_64-linux-musl (gcc)** with the following capabilities:
    - Use `-ansic` and `-std=c90` for ISO C compliance.
    - Define `_POSIX_C_SOURCE=1` and `_XOPEN_SOURCE=500` for POSIX and X/Open features supported by musl.
    - Avoid advanced GNU and GCC-specific extensions unsupported by musl.
- **Libraries**:
    - Base code on **musl libc** for portability and minimalism.
    - Ensure compatibility with musl's threading and math libraries.
- **Containerized Development**:
    - Target **AMD64** architecture with static linking for compatibility across container instances.
    - Optimize for minimal dependencies, leveraging musl’s built-in features where possible.

## **Compiler Flags Reference**
### Production Build (musl-gcc because it runs on Trisquel)
```c
CFLAGS = -std=c90 -D_POSIX_C_SOURCE=1 -D_XOPEN_SOURCE=500 -Wall -Wextra -pedantic -static
LDFLAGS = -static -pthread -Wl,-O1 -Wl,--gc-sections -lssl -lcrypto
```

```c
### **Development Build (gcc as it ise alpine)**
CFLAGS = -std=c90 -D_POSIX_C_SOURCE=1 -D_XOPEN_SOURCE=500 -Wall -Wextra -pedantic -O0 -ggdb3
LDFLAGS = -static -pthread -lssl -lcrypto
```
/lib/ld-musl-x86_64.so.1: This is the dynamic linker (or loader) for musl libc, which is responsible for loading shared libraries and starting the program.

/lib/libc.musl-x86_64.so.1: This is the musl C library itself, which provides the standard C library functions (e.g., malloc, printf, etc.).

## Explicit code block examples:
## **Code Structure Examples**

### Header File Template (.h)
```c
/* filepath: include/module_name.h */
#ifndef MODULE_NAME_H
#define MODULE_NAME_H

/* POSIX headers */
#include <sys/types.h>
#include <pthread.h>

/* Constants */
#define MODULE_MAX_SIZE 1024

/* Data structures */
struct module_data {
    int id;
    char name[MODULE_MAX_SIZE];
};

/* Function prototypes */
int moduleFunction(const struct module_data *data);

#endif /* MODULE_NAME_H */
```

### Source File Template (.c)
```c
/* filepath: src/module_name.c */
#include "module_name.h"
#include <stdlib.h>
#include <string.h>

int moduleFunction(const struct module_data *data)
{
    if (data == NULL)
        return -1;
    return 0;
}
```

## Example Compliant COde
```c
/* filepath: example.h */
#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <sys/types.h>
#include <unistd.h>

/* Constants */
#define MAX_BUF 1024

/* Types */
struct data_block {
    int id;
    char buf[MAX_BUF];
};

/* Function prototypes */
int
process_data(struct data_block *block);

#endif /* EXAMPLE_H */
```
```c
/* filepath: example.c */
#include "example.h"
#include <errno.h>
#include <stdlib.h>

int
process_data(struct data_block *block)
{
    int result;
    char *temp;

    if (block == NULL) {
        errno = EINVAL;
        return -1;
    }

    temp = malloc(MAX_BUF);
    if (temp == NULL) {
        return -1;
    }

    /* Process data */
    result = do_work(temp, block->buf);

    free(temp);
    return result;
}
```

## Build target clarity:
```markdown
## **Build Targets**
- `make ENV=dev` - Development build with debug symbols
- `make ENV=prod` - Production build with optimizations
- `make install` - Install based on ENV
- `make clean` - Clean build artifacts
```
---

## **Testing and Validation**
- Write unit tests using CUNit framework for all functions using the following naming conventions:
    - `test_<module_name>.c` for test files.
    - `test_<function_name>` for individual test cases.
- Use memory analysis tools (e.g., `valgrind` or equivalent musl-compatible tools) to validate memory safety.
- Run all tests within the containerized environment to ensure consistency.

## Style and Formatting
### Indentation
- Use spaces (not tabs) for indentation, with an indent size of 4.

### Brace Style
- Use the Allman brace style.

### Quotes
- Prefer double quotes for strings in C and header files.

### Additional Formatting Rules
- Align consecutive assignments and declarations.
- Limit lines to 80 characters for readability.

## Documentation
- Document all files and functions using C90 comments style.
- Use Doxygen for external documentation.

## Tools and Practices
- Use Git for version control, and ensure commit messages follow the format: `[Type] Short description`.
- CUnit framework for testing
- Use CI/CD pipelines to automate testing and code quality checks.
- Regularly review and refactor code for maintainability and performance.

## Copilot Usage Guidelines
- Focus on providing code snippets that can be copied into existing files rather than rewriting entire files.
- Ensure that the generated snippets adhere to the project's coding standards and practices as outlined in this document.
- Provide context-specific suggestions that integrate seamlessly with the existing codebase.

Here’s a comprehensive list of the **POSIX-compliant** and **ISO C89 (ANSI C)** headers and libraries provided by **musl libc**. These libraries and headers are essential for building a project that adheres to your requirements for compliance, portability, and lightweight environments.

---

## **ISO C89 Standard Headers**
These headers are defined by the **ISO/IEC 9899:1990 (C89)** standard and are fully supported by musl libc.

### **Standard Library Headers**
| Header         | Description                                                                 |
|----------------|-----------------------------------------------------------------------------|
| `<assert.h>`   | Diagnostics: `assert` macro for runtime error checking.                    |
| `<ctype.h>`    | Character classification and conversion functions.                        |
| `<errno.h>`    | Error codes used by library functions.                                     |
| `<float.h>`    | Defines limits for floating-point types.                                   |
| `<limits.h>`   | Defines limits for integral types.                                         |
| `<locale.h>`   | Localization functions and macros.                                         |
| `<math.h>`     | Mathematical functions.                                                   |
| `<setjmp.h>`   | Non-local jumps (e.g., `setjmp` and `longjmp`).                            |
| `<signal.h>`   | Signal handling.                                                          |
| `<stdarg.h>`   | Variable argument list handling (`va_list`, `va_start`, etc.).            |
| `<stddef.h>`   | Common definitions (e.g., `size_t`, `NULL`, `offsetof`).                  |
| `<stdio.h>`    | Input/output functions (e.g., `printf`, `fopen`, `fread`).                |
| `<stdlib.h>`   | General utilities (e.g., memory management, random numbers).              |
| `<string.h>`   | String handling and memory manipulation functions.                        |
| `<time.h>`     | Time and date functions.                                                  |
| `<wchar.h>`    | Wide-character functions.                                                 |
| `<wctype.h>`   | Wide-character classification and conversion functions.                   |

---

## **POSIX Standard Headers**
Defined by **POSIX.1 (IEEE Std 1003.1)**, these headers extend the functionality of the ISO C standard and are fully supported by musl libc.

### **System Interfaces**
| Header           | Description                                                                 |
|------------------|-----------------------------------------------------------------------------|
| `<aio.h>`        | Asynchronous I/O.                                                         |
| `<dirent.h>`     | Directory entry manipulation (`opendir`, `readdir`, etc.).                |
| `<fcntl.h>`      | File control options (`open`, `fcntl`).                                   |
| `<grp.h>`        | Group database access (`getgrgid`, `getgrnam`).                           |
| `<pwd.h>`        | User database access (`getpwuid`, `getpwnam`).                            |
| `<sys/stat.h>`   | File status (`stat`, `fstat`, etc.).                                       |
| `<sys/types.h>`  | Data types used in system calls (`pid_t`, `off_t`, etc.).                 |
| `<sys/wait.h>`   | Process termination (`wait`, `waitpid`).                                  |
| `<termios.h>`    | Terminal I/O interfaces.                                                  |
| `<unistd.h>`     | POSIX API for system services (`fork`, `exec`, `read`, `write`, etc.).    |

### **Signals and Threads**
| Header           | Description                                                                 |
|------------------|-----------------------------------------------------------------------------|
| `<pthread.h>`    | POSIX threads (Pthreads) interface.                                        |
| `<signal.h>`     | Signal handling and manipulation.                                          |
| `<sys/signal.h>` | Signal-related constants and macros (alias of `<signal.h>`).               |

### **Networking and IPC**
| Header           | Description                                                                 |
|------------------|-----------------------------------------------------------------------------|
| `<arpa/inet.h>`  | Internet operations (`inet_ntoa`, `inet_addr`).                            |
| `<netdb.h>`      | Network database operations (`gethostbyname`, `getaddrinfo`).              |
| `<netinet/in.h>` | IPv4 and IPv6 definitions.                                                 |
| `<sys/socket.h>` | Socket programming interface.                                              |
| `<sys/un.h>`     | UNIX domain sockets.                                                      |
| `<sys/ipc.h>`    | Interprocess communication (IPC) definitions.                              |
| `<sys/msg.h>`    | Message queue operations.                                                 |
| `<sys/sem.h>`    | Semaphore operations.                                                     |
| `<sys/shm.h>`    | Shared memory operations.                                                 |

### **Advanced File I/O**
| Header           | Description                                                                 |
|------------------|-----------------------------------------------------------------------------|
| `<sys/mman.h>`   | Memory management (`mmap`, `munmap`).                                      |
| `<sys/statvfs.h>`| Filesystem statistics (`statvfs`, `fstatvfs`).                             |
| `<utime.h>`      | File access and modification times.                                        |

---

## **Additional POSIX and X/Open 500 Headers**
These headers are part of the **X/Open 500** standard, which extends POSIX functionality.

| Header           | Description                                                                 |
|------------------|-----------------------------------------------------------------------------|
| `<regex.h>`      | Regular expression handling.                                               |
| `<fnmatch.h>`    | Filename matching (`fnmatch`).                                             |
| `<tar.h>`        | Tar file format definitions.                                               |
| `<fts.h>`        | File traversal utility functions.                                          |
| `<libgen.h>`     | Path and filename manipulation (`basename`, `dirname`).                   |
| `<utmp.h>`       | User accounting database.                                                 |
| `<wordexp.h>`    | Word expansion (e.g., globbing).                                           |

---

## **Math and Floating-Point Extensions**
| Header           | Description                                                                 |
|------------------|-----------------------------------------------------------------------------|
| `<fenv.h>`       | Floating-point environment control.                                        |
| `<tgmath.h>`     | Type-generic macros for mathematical functions.                           |

---

## **musl-Specific Features**
While musl libc is designed to strictly follow standards, it also includes enhancements for better performance and compatibility:
- **Static linking support**: Musl is optimized for creating fully static binaries.
- **Thread safety**: Full support for POSIX threads (Pthreads).
- **Lightweight implementations**: Many functions are rewritten to be smaller and faster compared to glibc.

---
