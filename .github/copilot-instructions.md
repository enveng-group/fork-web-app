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

## Optimised Directory Structyre

```plaintext
/devcontainer/web-app/
├── bin/                    # Binaries
│   └── app
├── etc/                    # Configuration
│   ├── auth/passwd        # Auth data
│   ├── env/.env          # Environment config
│   └── ssl/              # SSL certificates
│       ├── cert.pem
│       └── privkey.pem
├── include/               # Header files
├── lib/                   # Libraries
├── obj/                   # Object files
├── src/                   # Source files
├── static/                # Static web files
├── tests/                 # Test files
└── var/                   # Variable data
    ├── lib/              # Application data
    │   ├── database.descr
    │   └── records.rec
    └── log/              # Logs
        ├── app.log
        └── audit.log
```

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
### Production Build (musl-gcc)
```c
CFLAGS = -std=c90 -D_POSIX_C_SOURCE=1 -D_XOPEN_SOURCE=500 -Wall -Wextra -pedantic -static
LDFLAGS = -static -pthread -Wl,-O1 -Wl,--gc-sections -lssl -lcrypto
```

```c
### **Development Build (musl-gcc)**
CFLAGS = -std=c90 -D_POSIX_C_SOURCE=1 -D_XOPEN_SOURCE=500 -Wall -Wextra -pedantic -O0 -ggdb3
LDFLAGS = -static -pthread -lssl -lcrypto
```

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
- Write unit tests for all functions using the following naming conventions:
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
- Use CI/CD pipelines to automate testing and code quality checks.
- Regularly review and refactor code for maintainability and performance.

## Copilot Usage Guidelines
- Focus on providing code snippets that can be copied into existing files rather than rewriting entire files.
- Ensure that the generated snippets adhere to the project's coding standards and practices as outlined in this document.
- Provide context-specific suggestions that integrate seamlessly with the existing codebase.

## basic directory structure

```plaintext
.
├── ACKNOWLEDGEMENT.md
├── AUTHORS
├── CHANGELOG
├── CODE_OF_CONDUCT.md
├── CONTRIBUTING.md
├── COPYING
├── HACKING
├── INSTALL
├── LICENSE
├── MAINTAINERS
├── MONITORING_AND_EVALUATION
├── Makefile
├── README.md
├── SECURITY.md
├── SUPPORT.md
├── TODO.md
├── bin
│   └── app
├── docs
│   ├── ARCHITECTURE.md
│   ├── CONTACT.md
│   ├── CONTRIBUTORS.md
│   ├── CREDITS.md
│   ├── DEVELOPERS.md
│   ├── DISCLAIMER.md
│   ├── EXAMPLES.md
│   ├── FAQ.md
│   ├── GLOSSARY.md
│   ├── GOVERNANCE.md
│   ├── HISTORY.md
│   ├── MIGRATION.md
│   ├── PRIVACY.md
│   ├── REFERENCES.md
│   ├── RELEASE_NOTES.md
│   ├── REQUIREMENTS.md
│   ├── ROADMAP.md
│   ├── STYLEGUIDE.md
│   ├── TERMS.md
│   ├── TROUBLESHOOTING.md
│   ├── TUTORIAL.md
│   └── USAGE.md
├── etc
│   ├── auth
│   │   └── passwd
│   ├── env
│   └── ssl
│       ├── cert.pem
│       └── privkey.pem
├── include
│   ├── audit.h
│   ├── audit_events.h
│   ├── auth.h
│   ├── config.h
│   ├── data_structures.h
│   ├── database.h
│   ├── http.h
│   ├── logger.h
│   ├── quic_module.h
│   ├── server.h
│   ├── session.h
│   ├── ssl_module.h
│   └── utils.h
├── lib
├── make
│   ├── Makefile.common
│   ├── Makefile.dev
│   └── Makefile.prod
├── obj
│   ├── audit.o
│   ├── audit_events.o
│   ├── auth.o
│   ├── config.o
│   ├── data_structures.o
│   ├── database.o
│   ├── http.o
│   ├── logger.o
│   ├── main.o
│   ├── quic_module.o
│   ├── server.o
│   ├── session.o
│   ├── ssl_module.o
│   └── utils.o
├── scripts
│   └── setup
│       ├── dev.sh
│       └── prod.sh
├── src
│   ├── audit.c
│   ├── audit_events.c
│   ├── auth.c
│   ├── config.c
│   ├── data_structures.c
│   ├── database.c
│   ├── http.c
│   ├── logger.c
│   ├── main.c
│   ├── quic_module.c
│   ├── server.c
│   ├── session.c
│   ├── ssl_module.c
│   └── utils.c
├── static
│   ├── dashboard.html
│   ├── index.html
│   └── profile.html
├── tests
├── var
│   ├── lib
│   │   ├── database.descr
│   │   └── records.rec
│   └── log
│       ├── app.log
│       └── audit.log
└── web-app.code-workspace

19 directories, 96 files
```

### POSIX Headers only allowed to use
<aio.h>
<arpa/inet.h>
<assert.h>
<complex.h>
<cpio.h>
<ctype.h>
<dirent.h>
<dlfcn.h>
<errno.h>
<fcntl.h>
<fenv.h>
<float.h>
<fnmatch.h>
<ftw.h>
<glob.h>
<grp.h>
<iconv.h>
<inttypes.h>
<iso646.h>
<langinfo.h>
<libgen.h>
<limits.h>
<locale.h>
<math.h>
<monetary.h>
<mqueue.h>
<net/if.h>
<netdb.h>
<netinet/in.h>
<netinet/tcp.h>
<nl_types.h>
<poll.h>
<pthread.h>
<pwd.h>
<regex.h>
<sched.h>
<search.h>
<semaphore.h>
<setjmp.h>
<signal.h>
<stdarg.h>
<stdbool.h>
<stddef.h>
<stdint.h>
<stdio.h>
<stdlib.h>
<string.h>
<strings.h>
<sys/ipc.h>
<sys/mman.h>
<sys/msg.h>
<sys/resource.h>
<sys/select.h>
<sys/sem.h>
<sys/shm.h>
<sys/socket.h>
<sys/stat.h>
<sys/statvfs.h>
<sys/time.h>
<sys/times.h>
<sys/types.h>
<sys/uio.h>
<sys/un.h>
<sys/utsname.h>
<sys/wait.h>
<syslog.h>
<termios.h>
<tgmath.h>
<time.h>
<ulimit.h>
<unistd.h>
<utime.h>
<wchar.h>
<wctype.h>
<wordexp.h>
