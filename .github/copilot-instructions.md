# Copilot Instructions for EnvEng Web Application Development

These instructions help align Copilot's suggestions with our project's requirements.

## General Guidelines
- Adhere to ISO/IEC 9899:2024 (C Standard), POSIX.1-2024, and X/Open 800 compliance in all code suggestions.
- Ensure all code is portable, cross-platform, and cross-architecture. Avoid platform-specific features unless encapsulated for easy replacement.

## Code Structure
### File Organization
- Maintain the specified directory structure.
- Use snake_case for file names, e.g., `data_manager.c`, `user_auth.h`.

### Naming Conventions
- **Variables**: Use `snake_case` (e.g., `user_id`).
- **Functions**: Use `camelCase` (e.g., `processData`).
- **Constants**: Use `UPPER_CASE` (e.g., `MAX_BUFFER_SIZE`).

## Coding Practices
### Data Handling
- Keep data definitions separate from logic. Define data structures in header files and implement logic in C files.
- Use immutable data structures whenever applicable.

### Error Handling
- Always check return values of system calls and library functions.

### Memory Management
- Avoid memory leaks. Always pair memory allocation (`malloc`) with deallocation (`free`).

### Performance
- Optimize for CPU cache efficiency with data structures (e.g., prefer arrays over linked lists for sequential access).

### Thread Safety
- Ensure thread safety using POSIX threading primitives (e.g., `pthread_mutex_t`).

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
- Document all files and functions using block comments.
- Use Markdown for external documentation.

## Testing
- Write unit tests for all functions and features.
- Follow the naming convention `test_<module_name>.c` for test files.

## Tools and Practices
- Use Git for version control, and ensure commit messages follow the format: `[Type] Short description`.
- Use CI/CD pipelines to automate testing and code quality checks.
- Regularly review and refactor code for maintainability and performance.

## Copilot Usage Guidelines
- Focus on providing code snippets that can be copied into existing files rather than rewriting entire files.
- Ensure that the generated snippets adhere to the project's coding standards and practices as outlined in this document.
- Provide context-specific suggestions that integrate seamlessly with the existing codebase.

### File Organization

- Maintain the following directory structure:

```
.
├── ACKNOWLEDGEMENT.md
├── AUTHORS
├── CHANGELOG.md
├── CODE_OF_CONDUCT.md
├── CONTRIBUTING.md
├── COPYING
├── ChangeLog
├── HACKING
├── INSTALL
├── LICENSE
├── MAINTAINERS
├── MONITORING_AND_EVALUATION
├── Makefile
├── NEWS
├── README.md
├── SECURITY.md
├── SUPPORT.md
├── TODO.md
├── bin
│   └── my_program.1.pdf
├── build
│   ├── configurationCache.log
│   ├── dryrun.log
│   └── targets.log
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
│   ├── INSTALL.md
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
│   ├── USAGE.md
│   └── my_program.1
├── etc
│   ├── config.conf
│   ├── config.ini
│   ├── server
│   └── ssl
│       └── enssol.com.au
├── include
│   ├── config_loader.h
│   ├── env_loader.h
│   ├── error_handler.h
│   ├── garbage_collector.h
│   ├── hello.h
│   ├── logger.h
│   └── validator.h
├── lib
│   ├── ctype.h
│   ├── stdarg.h
│   ├── stdio.h
│   ├── stdlib.h
│   └── string.h
├── src
│   ├── config_loader.c
│   ├── env_loader.c
│   ├── error_handler.c
│   ├── garbage_collector.c
│   ├── hello.c
│   ├── logger.c
│   ├── main.c
│   └── validator.c
├── tests
│   └── test_logger.c
├── tmp
│   └── http.pseudo
├── var
│   └── www
│       ├── data
│       └── html
└── web-app.code-workspace

17 directories, 71 files
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
<fmtmsg.h>
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
<ndbm.h>
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
<spawn.h>
<stdarg.h>
<stdbool.h>
<stddef.h>
<stdint.h>
<stdio.h>
<stdlib.h>
<string.h>
<strings.h>
<stropts.h>
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
<tar.h>
<termios.h>
<tgmath.h>
<time.h>
<trace.h>
<ulimit.h>
<unistd.h>
<utime.h>
<utmpx.h>
<wchar.h>
<wctype.h>
<wordexp.h>
