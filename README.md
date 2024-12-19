# EnvEng Web Server

A lightweight HTTP web server implementation in ANSI C (C89), focusing on POSIX compliance and minimal dependencies.

## Features
- Static file serving over HTTP/1.0
- Basic authentication and session management
- POSIX-compliant, musl libc based implementation
- Minimal footprint with static binary output

## Prerequisites
- gcc compiler
- musl libc
- GNU Make
- CUnit testing framework

## Building

### Production Build
```bash
make clean && make
```

### Run Tests
```bash
make clean && make test
```

## Project Structure
```
.
├── bin/                # Binary outputs
├── include/           # Header files
├── src/              # Source code
├── test/             # Test suite
└── www/              # Static web content
```

## Usage
Start the server:
```bash
./bin/web_server
```

Access via browser:
- Login page: http://localhost:8080
- Test page: http://localhost:8080/test_index.html

## Technical Details
- Compliant with ISO/IEC 9899:1990 (C90)
- POSIX.1 (IEEE 1003.1) compliance
- Static linking with musl libc
- Thorough error handling

## License
AGPL-3.0-or-later
