# EnvEng Web Server

A lightweight HTTP web server implementation in ANSI C (C89), focusing on POSIX compliance and minimal dependencies.

## Features
- Static file serving over HTTP/1.0
- Basic authentication and session management
- POSIX-compliant, musl libc based implementation
- Minimal footprint with static binary output
- ISO-8859-1 encoding support
- HTML4.01 Strict compliant
- CSS2.1 compliant
- ECMAScript 3 (ES3) compliant
- Mobile-first responsive design

## Prerequisites
- musl-gcc compiler
- musl libc
- GNU Make
- CUnit testing framework

## Build Commands

### Basic Build Commands
```bash
make                # Default build (verifies musl and builds production)
make prod          # Build production binary
make test          # Build and run tests with coverage
make check         # Build and run tests without coverage
make debug         # Build tests and launch GDB debugger
make clean         # Clean build artifacts

# Clean previous builds and create new release package
make release

# Create t4g-optimized release
make t4g-release

# The optimized package will be at:
# dist/web-app-0.0.1.tar.gz
```

## Project Structure
```
.
??? bin/                # Binary outputs
??? etc/               # Configuration files
??? include/           # Header files
??? lib/               # Libraries
??? src/              # Source code
??? test/             # Test suite
??? www/              # Static web content
    ??? assets/       # Static assets
    ??? error/        # Error pages
    ??? templates/    # HTML templates
```

## Usage
Start the server:
```bash
./bin/web_server -c /etc/web_server.conf
```

Access via browser:
- Login page: http://localhost:8080
- Admin panel: http://localhost:8080/admin
- Status page: http://localhost:8080/status

## Technical Details
- ISO/IEC 9899:1990 (C90) compliant
- POSIX.1 (IEEE 1003.1) compliant
- X/Open-500 compliant
- Static linking with musl libc
- ISO-8859-1 character encoding
- Memory-safe implementation
- Thread-safe operations
- No external dependencies

## Development Guidelines
- Mobile-first responsive design
- HTML 4.01 Strict / XHTML 1.0 Strict
- Native CSS2.1 only (no frameworks)
- ES3 JavaScript (no libraries)
- ISO-8859-1 encoding for all files

## Security
- Memory protection features
- Stack protection enabled
- Strict input validation
- Safe string handling
- Format string protection

## License
AGPL-3.0-or-later
