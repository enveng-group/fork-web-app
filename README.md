# Web-App

##

### Project overview

The **WebApp** is designed to provide an efficient, scalable, and maintainable web application tailored to environmental engineering needs. The key goals of this project include:

- **Building a scalable, maintainable codebase.**
- **Ensuring clarity through Data-Oriented Programming (DOP).**
- **Maintaining an organised and efficient development process using 5S methodology.**

#### Key Objectives

- Implement user authentication and core features.
- Optimise data handling and performance.
- Maintain clarity and organisation throughout the project lifecycle.

---

### Table of Contents

1. [[Project Overview](#project-overview)](#Project-overview)
2. [[Features](#features)](#Features)
3. [[Methodologies](#methodologies)](#Methodologies)
    - [[5S Implementation](#5s-implementation)](#5s-implementation)
    - [[Data-Oriented Programming](#data-oriented-programming)](#data-oriented-programming)
4. [[Set-Up Instructions](#set-up-instructions)](#set-up-instructions)
5. [[Git Workflow](#git-workflow)](#git-workflow)
    - [[Branching Strategy](#branching-strategy)](#branching-strategy)
    - [[Commit Guidelines](#commit-guidelines)](#commit-guidelines)
6. [[CI/CD Workflow](#cicd-workflow)](#cicd-workflow)
7. [[Contributing](#contributing)](#contributing)
8. [[Licence](#licence)](#licence)
9. [[Contact](#contact)](#contact)

---

### Features

- **User Authentication and Authorisation**: Secure login and user management system.
- **Dashboard**: Manage and visualise environmental data.
- **API Integrations**: Fetch real-time updates from third-party sources.
- **Performance Optimised**: Clean, maintainable, and efficient codebase ensuring smooth operation.

---

### Methodologies

Our team uses the **5S methodology** to keep the project organised and efficient:

- **Sort**: Remove unnecessary tasks, features, and code.
- **Set in Order**: Organise files, code, and tasks for easy access.
- **Shine**: Keep the code and project environment clean with regular reviews and automated checks.
- **Standardise**: Establish coding standards, guidelines, and reusable templates.
- **Sustain**: Encourage continuous improvement and regular audits to ensure long-term project health.

---

#### 5S Implementation

##### Sort

- Objective: Identify and eliminate unnecessary items from the project workspace.
- Actions:
Conduct an initial audit of all project resources (code, documents, tools).
Remove or archive obsolete or redundant items.

##### Set in Order

- Objective: Organize remaining items for easy access and efficiency.
- Actions:
Establish a logical structure for code repositories.
Create a standardized directory and file naming convention.
Implement version control practices.

##### Shine

- Objective: Maintain a clean and organized workspace.
- Actions:
Schedule regular code reviews and refactoring sessions.
Implement automated tools for code quality checks.

##### Standardize

- Objective: Establish standards for organization and cleanliness.
- Actions:
Develop coding standards and best practices documentation.
Create templates for common tasks and documentation.

##### Sustain

- Objective: Maintain and review standards consistently.
- Actions:
Conduct regular training sessions on 5S practices.
Schedule periodic audits to ensure compliance with standards.

#### Data-Oriented Programming

The **Data-Oriented Programming (DOP)** methodology is employed to enhance performance, maintainability, and scalability:

- **Data Separation**: Separate data from application logic to maintain clean, reusable code.
- **Efficient Data Handling**: Use optimised data structures that ensure high performance.
- **Immutability**: Follow immutable data practices to ensure consistency across the application.

---

### Set-Up Instructions

Follow these steps to get the development environment up and running:

1. **Clone the repository**

   ```sh
   git clone <https://github.com/your-org/web-app.git>
   cd web-app
   ```

### Git Workflow

To ensure collaboration and maintain project quality, we follow a structured Git workflow:

---

#### Branching Strategy

- Main: The stable, production-ready branch.
- Develop: The integration branch for new features and fixes.
- Feature Branches: Use feature/feature-name for new features.
- Bugfix Branches: Use bugfix/bug-description for bug fixes.

### Commit Guidelines

- Write atomic commits focused on a single change or fix.
- Use descriptive commit messages to ensure clarity.
    - Example:

        - git commit -m "Add user authentication endpoint"
        - CI/CD Workflow

We use Continuous Integration (CI) and Continuous Delivery (CD) to automate testing and deployment.

---

### CI/CD Workflow

#### Continuous Integration (CI)

- Automated builds and tests run for every push to the develop branch.
Static code analysis is used to maintain code quality.

#### Continuous Delivery (CD)

- Deployments to a staging environment are automated for integration testing.
Production deployment requires manual approval.
- Pipeline Example
    - Refer to the .github/workflows/ci-cd.yml file for detailed CI/CD pipeline configuration.

---

### Contributing

We welcome contributions to improve the WebApp. If you'd like to contribute, follow these steps:

Fork the repository and clone it to your local machine.
Create a new feature or bugfix branch.
Make your changes, ensuring your commits follow the commit guidelines.
Submit a pull request to the develop branch for review.
For more information, refer to our [CONTRIBUTING.md](CONTRIBUTING.md) file.

---

### Licence

This project is licensed under the [LICENSE](LICENSE). Please review it for details on usage and distribution.

---

### Contact

For any inquiries or questions regarding this project, feel free to reach out to the team:

Lead Developer: [Adrian Gallo](agallo@enveng-group.com.au)

Project Maintainer: [Adrian Gallo](agallo@enveng-group.com.au)


## General Guidelines**

- Adhere to **ISO/IEC 9899:2024** (C Standard), **POSIX.1-2024**, and **X/Open 800** compliance in all code suggestions.
- Ensure all code is portable, cross-platform, and cross-architecture. Avoid platform-specific features unless encapsulated for easy replacement.


- Use **snake_case** for file names, e.g., `data_manager.c`, `user_auth.h`.

### Naming Conventions

- **Variables**: Use `snake_case` (e.g., `user_id`).
- **Functions**: Use `camelCase` (e.g., `processData`).
- **Constants**: Use `UPPER_CASE` (e.g., `MAX_BUFFER_SIZE`).

---

## **3. Coding Practices**

### Data Handling (Aligned with Data-Oriented Programming)

- **Separation**: Keep data definitions separate from logic. Define data structures in header files and implement logic in C files.
- **Immutability**: Use immutable data structures whenever applicable.

### Error Handling

- Always check return values of system calls and library functions. Example:
    ```c
    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    ```

### Memory Management

- Avoid memory leaks. Always pair memory allocation (`malloc`) with deallocation (`free`).
- Use tools like `valgrind` for debugging memory issues.

### Performance

- Optimize for CPU cache efficiency with data structures (e.g., prefer arrays over linked lists for sequential access).

### Thread Safety

- Ensure thread safety using POSIX threading primitives (e.g., `pthread_mutex_t`).

---

## **4. Style and Formatting**

### Indentation

- Use spaces (not tabs) for indentation, with an indent size of 4.
- For Makefiles and Makefile.am files, use tabs for indentation with an indent size of 4.

### Brace Style

- Use the Allman brace style:
    ```c
    if (condition)
    {
        // Code block
    }
    ```

### Quotes

- Prefer double quotes for strings in C and header files.

### Additional Formatting Rules

- Align consecutive assignments and declarations.
- Allow short functions on a single line (inline only).
- Break before braces for functions, control statements, namespaces, classes, else, catch, while, structs, enums, and unions.
- Space before parentheses in control statements.
- No spaces in container literals.
- Sort includes and preserve include blocks.

### Line Length

- Limit lines to 80 characters for readability.

---

## **5. Documentation**

- Document all files and functions using block comments:

    ```c
    /**
     * Function: calculateSum
     * ----------------------
     * Computes the sum of two integers.
     *
     * x: First integer
     * y: Second integer
     *
     * returns: The sum of x and y
     */
    ```

- Use Markdown for external documentation and place it in the `docs/` directory.

---

## **6. Testing**

- Write unit tests for all functions and features.
- Follow the naming convention `test_<module_name>.c` for test files.

```sh
mkdir build
cd build
cmake -G Ninja ..
ninja
ctest
```

To ensure **POSIX compliance** while integrating the **musl C library** and statically linking everything into your program, follow these instructions. musl is a lightweight, fast, and POSIX-compliant C standard library that can replace glibc for static linking.

---

## **1. Prerequisites**

Install and ensure the availability of the following **POSIX-compliant tools** and libraries:

- **Compiler**: `clang` (set to use musl as the C library)
- **Build System**: `cmake`
- **Linker**: `lld`
- **Static Analysis**: `lint` (e.g., `splint`)
- **Formatting**: `indent`
- **Debugger**: `lldb`
- **Profiling**: `gprof` < -- TODO: Need to figure out how to download >
- **Memory Debugging**: `valgrind`
- **Documentation**: `groff`
- **Packaging**: `tar` and `xz`
- **musl**: The musl C library
- **bear**: To generate compilation database for clang-tidy

---

--

## **6. Building and Running**

After configuring the `Makefile` and ensuring musl is correctly set up, you can build and run your program as follows:

### **Build Commands**

```sh
# Build the project
make

# Clean the project
make clean

# Run tests
make test

# Generate documentation
make doc
```

---

## **7. Static Analysis and Formatting**

You can use **POSIX-compliant tools** for static analysis and formatting:

#### **POSIX Lint Tool**
```sh
splint src/*.c include/*.h
```

#### **POSIX Formatting Tool**
```sh
indent src/*.c include/*.h
```

---

## **8. Debugging and Profiling**

You can use **gdb** for debugging and **gprof** for profiling, even with musl:

#### **Debugging**
```sh
gdb ./bin/my_program
```

#### **Profiling**
```sh
gprof ./bin/my_program gmon.out > profile.txt
```

#### **Memory Debugging**
```sh
valgrind --leak-check=full ./bin/my_program
```

---

## **9. Documentation**

Generate **man pages** for POSIX compliance.

#### **Man Page Example: `docs/my_program.1`**

```man
.TH my_program 1 "November 2024" "1.0.0" "My C Project Manual"
.SH NAME
my_program \- Example POSIX-compliant program
.SH SYNOPSIS
.B my_program
.SH DESCRIPTION
An example program that demonstrates POSIX compliance using musl.
.SH AUTHOR
Written by Your Name.
.SH COPYRIGHT
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
```

#### **Generate Man Pages**

```sh
groff -man docs/my_program.1 > bin/my_program.1.pdf
```

---

## **10. Packaging for Distribution**

You can package the project with static libraries and all necessary files.

#### **Create Archive**

```sh
tar -czvf my_project.tar.gz bin include lib src
```

---

## **Conclusion**

This workflow ensures **POSIX compliance** and integrates **musl** as the C library, enabling static linking and providing a lightweight and efficient alternative to glibc. The build system uses Clang with musl, and all binaries are statically linked, ensuring portability and reducing external dependencies. Let me know if further refinements are needed!

`clang-format` `clang-tidy` `cppcheck` `shellcheck`

Development of the EnvEng web application using 5S methodology for project management and Data-Oriented Programming (DOP) for development.



## **Conclusion**

This workflow ensures **POSIX compliance** and integrates **musl** as the C library, enabling static linking and providing a lightweight and efficient alternative to glibc. The build system uses Clang with musl, and all binaries are statically linked, ensuring portability and reducing external dependencies. Let me know if further refinements are needed!

### Build Toolchain

The build process is managed using CMake with Ninja as the build system. The following targets are available:

- **clang-format**: Formats the source code according to the project's style guidelines.
- **clang-tidy**: Runs clang-tidy for static code analysis.
- **cppcheck**: Runs cppcheck for static code analysis.
- **valgrind**: Runs valgrind to check for memory leaks and other memory-related issues.
- **splint**: Runs splint for static code analysis.
- **clang-lint**: Runs clang-lint for static code analysis.
- **indent**: Formats the source code using indent.
- **doxygen** and **graphiz**: Generates PDF documentation from man pages.
- **gprof**: Runs gprof for profiling the application.
- **clang-analyzer**: Runs clang-analyzer for static code analysis.
- **llvm-cov**: Runs llvm-cov for code coverage analysis.
- **llvm-profdata**: Merges profiling data using llvm-profdata.
- **llvm-symbolizer**: Symbolizes addresses in the binary using llvm-symbolizer.
- **clang-check**: Runs clang-check for static code analysis.
- **shellcheck**: Runs shellcheck for shell script analysis.
- **bear**: Generates a compilation database using Bear.
- **package**: Packages the binary into a tar.xz archive.
- **all-tools**: Runs all the above tools and packages the binary.

To automate the build process and test individual steps, follow these instructions:

---

### **Automated Build Process**
Run these commands sequentially to perform a complete build and toolchain run in one go:
1. **Generate Build Files**:
   ```bash
   cmake -G Ninja -S . -B build
   ```
   This command sets up the build system and generates the required files in the `build/` directory.

2. **Build the Project**:
   ```bash
   ninja -C build
   ```
   Builds the `web_app` executable and places it in the `build/bin/` directory.

3. **Run All Tools**:
   ```bash
   ninja -C build all-tools
   ```
   Executes:
   - Static analysis (`clang-tidy` and `cppcheck`).
   - Memory profiling with `valgrind`.
   - Documentation generation with `doxygen`.
   - Packaging of the binary into a `.tar.xz` archive.

---

Here's a detailed sequence of **commands for each tool at every stage of C compilation and development**, ensuring thorough use of your toolchain. Each stage is designed for strict POSIX and ISO c90 compliance, with no redundancy or overlap.

---

## **1. Code Formatting & Linting**
Run these commands before writing or committing code to ensure style compliance.

```bash
# Format C code
clang-format -style=gnu -dump-config > .clang-format
clang-format -i <source_file>.c  # Inline modification
clang-format -i src/*.c include/*.h            # Format all C files in the directory

# Format shell scripts (if applicable)
shfmt -w <script>.sh  # Inline modification
shfmt -d <script>.sh  # Show diff without applying changes
```

---

## **2. Static Analysis**
Run static analysis tools early in development to catch potential errors.

```bash
# clang-tidy: Perform advanced checks
clang-tidy --verify-config
mkdir build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cd ..
clang-tidy -fix-errors -header-filter=.* include/atomic_ops.h -- build
clang-tidy -fix-errors -header-filter=.* include/compat.h -- build
clang-tidy -fix-errors -header-filter=.* include/compat_layer.h -- build
clang-tidy -fix-errors -header-filter=.* include/compat_types.h -- build
clang-tidy -fix-errors -header-filter=.* include/config_loader.h -- build
clang-tidy -fix-errors -header-filter=.* include/constants.h -- build
clang-tidy -fix-errors -header-filter=.* include/csv_utils.h -- build
clang-tidy -fix-errors -header-filter=.* include/data_structures.h -- build
clang-tidy -fix-errors -header-filter=.* include/env_loader.h -- build
clang-tidy -fix-errors -header-filter=.* include/error_codes.h -- build
clang-tidy -fix-errors -header-filter=.* include/error_handler.h -- build
clang-tidy -fix-errors -header-filter=.* include/external_includes.h -- build
clang-tidy -fix-errors -header-filter=.* include/garbage_collector.h -- build
clang-tidy -fix-errors -header-filter=.* include/http_parser.h -- build
clang-tidy -fix-errors -header-filter=.* include/http_response.h -- build
clang-tidy -fix-errors -header-filter=.* include/logger.h -- build
clang-tidy -fix-errors -header-filter=.* include/project.h -- build
clang-tidy -fix-errors -header-filter=.* include/records.h -- build
clang-tidy -fix-errors -header-filter=.* include/rec_utils.h -- build
clang-tidy -fix-errors -header-filter=.* include/server.h -- build
clang-tidy -fix-errors -header-filter=.* include/socket_module.h -- build
clang-tidy -fix-errors -header-filter=.* include/ssl_module.h -- build
clang-tidy -fix-errors -header-filter=.* include/static_file_handler.h -- build
clang-tidy -fix-errors -header-filter=.* include/test_framework.h -- build
clang-tidy -fix-errors -header-filter=.* include/utils.h -- build
clang-tidy -fix-errors -header-filter=.* include/validator.h -- build

clang-tidy -fix-errors -header-filter=.* src/add_custom_uint64.s -- build
clang-tidy -fix-errors -header-filter=.* src/atomic_ops.c -- build
clang-tidy -fix-errors -header-filter=.* src/compat.c -- build
clang-tidy -fix-errors -header-filter=.* src/compat_layer.c -- build
clang-tidy -fix-errors -header-filter=.* src/compatStrtol64_asm.s -- build
clang-tidy -fix-errors -header-filter=.* src/config_loader.c -- build
clang-tidy -fix-errors -header-filter=.* src/constants.c -- build
clang-tidy -fix-errors -header-filter=.* src/csv_utils.c -- build
clang-tidy -fix-errors -header-filter=.* src/data_structures.c -- build
clang-tidy -fix-errors -header-filter=.* src/div_custom_uint64.s -- build
clang-tidy -fix-errors -header-filter=.* src/env_loader.c -- build
clang-tidy -fix-errors -header-filter=.* src/error_handler.c -- build
clang-tidy -fix-errors -header-filter=.* src/garbage_collector.c -- build
clang-tidy -fix-errors -header-filter=.* src/http_parser.c -- build
clang-tidy -fix-errors -header-filter=.* src/http_response.c -- build
clang-tidy -fix-errors -header-filter=.* src/logger.c -- build
clang-tidy -fix-errors -header-filter=.* src/main.c -- build
clang-tidy -fix-errors -header-filter=.* src/records.c -- build
clang-tidy -fix-errors -header-filter=.* src/rec_utils.c -- build
clang-tidy -fix-errors -header-filter=.* src/server.c -- build
clang-tidy -fix-errors -header-filter=.* src/sha256_asm.c -- build
clang-tidy -fix-errors -header-filter=.* src/socket_module.c -- build
clang-tidy -fix-errors -header-filter=.* src/ssl_module.c -- build
clang-tidy -fix-errors -header-filter=.* src/static_file_handler.c -- build
clang-tidy -fix-errors -header-filter=.* src/utils.c -- build
clang-tidy -fix-errors -header-filter=.* src/validator.c -- build
.....


# cppcheck: Find bugs in C code
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/config_loader.c
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/constants.c
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/env_loader.c
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/error_handler.c
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/garbage_collector.c
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/logger.c
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/main.c
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/utils.c
cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force src/validator.c

# splint: Strict C linting TODO: Need to figure out how to refactor without breaking code and then incorporate in cmake
splint src/config_loader.c
splint src/constants.c
splint src/env_loader.c
splint src/error_handler.c
splint src/garbage_collector.c
splint src/logger.c
splint src/main.c
splint src/utils.c
splint src/validator.c
splint *.c  # Analyze multiple files

# shellcheck: Analyze shell scripts
shellcheck <script>.sh
```

---

## **3. Documentation** TODO go through Doxygen in more detail and implement and add to Cmake
Use these tools to generate project documentation from your source code.

```bash
# Generate Doxygen configuration file (one-time setup)
doxygen -g Doxyfile

# Customize Doxyfile (edit manually, then generate documentation)
doxygen

# Generate diagrams using Graphviz (works with Doxygen)
dot -Tpng -o callgraph.png callgraph.dot  # Example of graph rendering
```

---

## **4. Build Configuration**
Set up and configure your build system.

```bash
# Generate build files using CMake with Ninja
cmake -G Ninja . -B build
cmake -G Ninja -DCMAKE_C_COMPILER=musl-gcc ..

# Build project
ninja

# Generate compilation database (needed for clang tools)
bear -- ninja
```

---

## **5. Debugging**
Debug your application at runtime.

```bash
# Debug program with GDB
gdb ./<binary>  # Launch debugger

# Use addr2line to convert addresses into file and line numbers
addr2line -e ./<binary> <address>

# Use llvm-symbolizer for symbol resolution
llvm-symbolizer -e ./<binary> <address>
```

---

## **6. Memory & Performance Analysis**
Analyze memory usage and optimize performance.

```bash
# Valgrind: Detect memory issues
valgrind ./<binary>

# AddressSanitizer: Run memory checks (built with Clang)
clang -fsanitize=address -o <binary> <source_file>.c
./<binary>

# Perf: CPU profiling
perf record ./<binary>
perf report

# Gprof: Performance profiling
gcc -pg -o <binary> <source_file>.c
./<binary>
gprof ./<binary> gmon.out > analysis.txt

# Bolt: Optimize binary layout
llvm-bolt ./<binary> -o <optimized_binary>
```

---

## **7. Coverage**
Run coverage tools to measure test completeness.

```bash
# Compile with coverage support
gcc -fprofile-arcs -ftest-coverage -o <binary> <source_file>.c

# Run binary to collect coverage data
./<binary>

# Generate coverage report
gcov <source_file>.c
gcov-tool merge <coverage_file_1>.gcov <coverage_file_2>.gcov -o merged.gcov
```

---

## **8. Compilation Database**
Generate a compilation database to integrate with Clang tools.

```bash
# Generate compile_commands.json
bear -- ninja
```

---

## **9. Packaging**
Package the compiled binary for distribution.

```bash
# Package using CPack (requires configuration in CMakeLists.txt)
cpack

# Compress using tar and xz
tar -cf <package_name>.tar <directory>
xz <package_name>.tar
```

---

**Testing with

## **10. Workflow Summary**
Here’s a sequential workflow from code writing to packaging:

1. **Pre-code Setup**
   ```bash
   clang-format -i *.c
   ```

2. **Static Analysis**
   ```bash
   clang-tidy <source_file>.c -- -std=c90
   cppcheck --enable=all --std=c90 <source_file>.c
   splint <source_file>.c
   ```

3. **Build and Generate Database**
   ```bash
   cmake -G Ninja -DCMAKE_C_COMPILER=musl-gcc .
   ninja
   bear -- ninja
   ```

4. **Debugging**
   ```bash
   gdb ./<binary>
   ```

5. **Memory and Performance Analysis**
   ```bash
   valgrind ./<binary>
   perf record ./<binary>
   ```

6. **Documentation**
   ```bash
   doxygen
   ```

7. **Packaging**
   ```bash
   cpack
   tar -cf <package_name>.tar <directory>
   xz <package_name>.tar
   ```

---

Let me know if you'd like an automated shell script for this workflow!

This structure supports both automation and manual testing, allowing you to pinpoint and address issues efficiently. Let me know if you need further clarification!

./configure --prefix=/usr/local/bin/musl CC=clang CXX=clang++ LD=lld --enable-static --disable-shared --disable-werror --enable-fts --enable-fpic --disable-nls --enable-debug --with-arch=native --with-tune=native --disable-multilib --enable-posix --enable-isoc90 --with-ld=lld


To automate the build process and test individual steps, follow these instructions:

---
### **Automated Build Process**
Run these commands sequentially to perform a complete build and toolchain run in one go:
1. **Generate Build Files**:
   ```bash
   cmake -G Ninja -S . -B build
   ```
   This command sets up the build system and generates the required files in the `build/` directory.

2. **Build the Project**:
   ```bash
   ninja -C build
   ```
   Builds the `web_app` executable and places it in the `build/bin/` directory.

3. **Run All Tools**:
   ```bash
   ninja -C build all-tools
   ```
   Executes:
   - Static analysis (`clang-tidy` and `cppcheck`).
   - Memory profiling with `valgrind`.
   - Documentation generation with `doxygen`.
   - Packaging of the binary into a `.tar.xz` archive.

---

### **Step-by-Step Testing**
To test each step individually, use the following commands:

1. **Build Only**:
   ```bash
   ninja -C build
   ```
   This builds the `web_app` executable.

2. **Static Analysis**:
   ```bash
   ninja -C build static-analysis
   ```
   Runs `clang-tidy` and `cppcheck`, saving logs to `build/logs/`.

3. **Memory Profiling**:
   ```bash
   ninja -C build memory-profiling
   ```
   Runs `valgrind` on the built binary and logs the results to `build/logs/valgrind.log`.

4. **Generate Documentation**:
   ```bash
   ninja -C build doc_doxygen
   ```
   Generates API documentation using `doxygen` and places it in the `build/` directory.

5. **Package the Binary**:
   ```bash
   ninja -C build package
   ```
   Creates a `.tar.xz` package containing the static binary.

---

### **Testing Compilation Database (Optional)**
For tools that require a compilation database:
```bash
ninja -C build bear
```
Generates the `compile_commands.json` in the `build/` directory for IDEs or additional tooling.

---

### **Logs and Outputs**
- Binaries: `build/bin/`
- Logs: `build/logs/`
- Documentation: `build/` (or as configured in the Doxygen file).
- Packaged Output: `build/web_app.tar.xz`.

---

This structure supports both automation and manual testing, allowing you to pinpoint and address issues efficiently. Let me know if you need further clarification!

### **Where to Save the Wrapper Script**

The wrapper script should be saved in a directory included in your `PATH` environment variable, ideally one that takes precedence over system directories like `/usr/bin`. Common locations are:

1. **Recommended Location**: `/usr/local/bin`
   - This directory is usually included in the `PATH` on most Linux systems and prioritized over `/usr/bin`.

2. **Alternative (User-Specific)**: `$HOME/bin`
   - If you don’t have root access, you can save it in your user-specific `bin` directory, and ensure `$HOME/bin` is added to your `PATH`.

---

### **How to Save the Script**

1. Create the script:
   ```bash
   sudo nano /usr/local/bin/musl-linker
   ```
   Add the following lines to the file:
   ```bash
   #!/bin/sh
   /usr/bin/musl-ldd "$@"
   ```
2. Make the script executable:
   ```bash
   sudo chmod +x /usr/local/bin/musl-linker
   ```

If saving to `$HOME/bin`:
```bash
mkdir -p $HOME/bin
nano $HOME/bin/musl-linker
chmod +x $HOME/bin/musl-linker
```

---

### **When to Execute**

The wrapper script does not need to be explicitly executed; it acts as a replacement for the linker binary (`/usr/bin/ld`). You reference it when configuring your build system, as follows:

1. **During CMake Configuration**:
   Use the wrapper script as the linker:
   ```bash
cmake -G Ninja -DCMAKE_C_COMPILER=musl-clang -B build
   ```

2. **Build Step**:
   Once CMake has been configured, just build normally using Ninja:
   ```bash
   ninja
   ```

---

### **Verifying the Wrapper Usage**

You can confirm the linker being used by checking the build logs or directly inspecting the `link.txt` files generated in the build directory:

```bash
grep "/usr/local/bin/musl-linker" <build-directory>/CMakeFiles/*/link.txt
```

This should list the `musl-linker` script being invoked for linking.

---

### **General Tips**

- **Global Persistence**: If you want all projects to use this wrapper without specifying it in CMake every time, you can export it in your shell:
  ```bash
  export LD=/usr/local/bin/musl-linker
  ```
  Add this line to your `.bashrc` or `.zshrc` for permanent effect.

- **Debugging**: Add debug messages to the script to verify it's being executed:
  ```bash
  echo "musl-linker invoked with: $@" >&2
  ```

**Cmake**

1. Generate Build Files:

```sh
cmake -DENABLE_DEV_CFLAGS=ON -G Ninja -S . -B build
```

2. Build the Project:

```sh
ninja -C build
```

3. Run All Tools:

```sh
ninja -C build run_all_tools
```

### Individual Build Targets

1. **Static Analysis**:

```sh
ninja -C build static-analysis
```

2. **Memory Profiling**:

```sh
ninja -C build memory-profiling
```

3. **Generate Documentation**:

```sh
ninja -C build doc_doxygen
```

4. **Package the Binary**:

```sh
ninja -C build package
```

5. **Run Tests**:

```sh
ninja -C build execute_tests
```

**SSL**

```sh
sudo apt-get install libssl-dev
mkdir -p /etc/letsencrypt/live/yourdomain.com
sudo cp /usr/include/x86_64-linux-musl /usr/local/include/
sudo cp /usr/include/openssl /usr/local/include/x86_64-linux-musl/
sudo cp /usr/include/x86_64-linux-gnu/openssl/opensslconf.h /usr/local/include/x86_64-linux-musl/openssl/
sudo cp /usr/include/x86_64-linux-gnu/openssl/oconfiguration.h /usr/local/include/x86_64-linux-musl/openssl/
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/letsencrypt/live/yourdomain.com/privkey.pem -out /etc/letsencrypt/live/yourdomain.com/cert.pem
```


***Hashing Passwords***

```sh
musl-clang -o hash_password hash_password.c -lssl -lcrypto
./hash_password "John Doe" "password123"
```


### **Setting Up User Authentication**
```sh
echo "username:password_hash:uid:gid:full_name:home_dir:shell" | sudo tee -a /devcontainer/web-app/etc/auth/passwd
```

```sh
# 1. Create volume if not exists
podman volume create pod_vol

# 2. Run container with volume mount
podman run --platform linux/amd64 \
  -it \
  --name web-app-container \
  -v pod_vol:/devcontainer:Z \
  -e HOME=/devcontainer \
  -w /devcontainer \
  -p 8080:80 \
  docker.io/enveng/web-app:latest \
  /bin/ash -c "source ~/.ashrc && exec /bin/ash"

# 3. In another terminal, verify mount
podman exec web-app-container ls -la /devcontainer

# 4. Stop container
podman stop web-app-container

# 5. Commit container with volume
podman commit \
  --format docker \
  --change "VOLUME /devcontainer" \
  --change "WORKDIR /devcontainer" \
  --change "ENV HOME=/devcontainer" \
  web-app-container \
  enveng/web-app:latest-with-volume

# 6. Save image
podman save -o web-app-with-volume.tar enveng/web-app:latest-with-volume

# 7. Load image on target system
podman load -i web-app-with-volume.tar
```

```sh
# .exrc
set noflash
set noai
set tabstop=4
set showmatch
set ignorecase
set noic
set sm
```

```sh
# .ashrc
# Interactive shell check
case "$-" in
    *i*) ;;
    *) return ;;
esac

# Environment variables
LANG=en_AU.UTF-8
LC_ALL=en_AU.UTF-8
EDITOR=vi
HISTSIZE=1000
HISTFILE="${HOME}/.ash_history"
PATH="${HOME}/bin:/usr/local/bin:${PATH}"

# Development environment
CC=musl-gcc
CFLAGS="-std=c90 -D_POSIX_C_SOURCE=1 -D_XOPEN_SOURCE=500 -Wall -Wextra -pedantic"
LDFLAGS="-static"

# Directory trimming function
polyglot_dirtrim() {
    dir="${PWD}"
    [ "$dir" = "${HOME}" ] && { printf "~"; return; }
    [ "${dir##*/}" = "" ] && dir="${dir%/}"

    elements="${1:-2}"
    count=$(printf "%s" "$dir" | tr -cd '/' | wc -c)

    if [ "$count" -le "$elements" ]; then
        printf "%s" "$dir"
    else
        printf ".../%s" "${dir##*/}"
    fi
}

# Git status function
polyglot_git_status() {
    branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null)
    [ -z "$branch" ] && return

    status=""
    git diff --quiet 2>/dev/null || status="${status}!"
    git diff --cached --quiet 2>/dev/null || status="${status}+"

    printf "(%s%s)" "$branch" "$status"
}

# Exit status display
polyglot_exit_status() {
    [ "$1" -ne 0 ] && printf "(%d) " "$1"
}

# Development aliases
alias ll='ls -l'
alias la='ls -la'
alias mk='make'
alias mkt='make test'
alias mkc='make clean'
alias gst='git status'
alias gl='git log --oneline'

# SSH agent setup
if [ -z "$SSH_AUTH_SOCK" ] && command -v dropbear-agent >/dev/null 2>&1; then
    eval "$(dropbear-agent)" >/dev/null 2>&1
    trap 'dropbear-agent -k' EXIT
fi

# Add SSH key if exists
if [ -f "${HOME}/.ssh/id_dropbear" ]; then
    dbclient-add "${HOME}/.ssh/id_dropbear" >/dev/null 2>&1
fi

# Set prompt
PS1='$(polyglot_exit_status $?)$(polyglot_dirtrim 2) $(polyglot_git_status) $ '

# Source local customizations
[ -f "${HOME}/.ashrc.local" ] && . "${HOME}/.ashrc.local"
```

```sh
# .gitconfig
[user]
    name =
    email =
    signingkey =

[core]
    editor = vi
    compression = 9
    whitespace = trailing-space,space-before-tab,tab-in-indent
    autocrlf = input
    eol = lf
    safecrlf = true
    fsmonitor = true
    sshCommand = "dbclient -y -i ~/.ssh/id_dropbear"
    # Add specific ignores for C development
    excludesfile = ~/.gitignore_global
    # Improve performance
    preloadindex = true
    fscache = true

[init]
    defaultBranch = main

[color]
    ui = auto
    status = auto
    diff = auto
    branch = auto

[http]
    # Optimize for Alpine/musl
    postBuffer = 157286400
    maxRequestBuffer = 100M

[ssh]
    variant = ssh

[gpg]
    program = gpg

[pull]
    rebase = true
    ff = only

[rebase]
    autoSquash = true
    autoStash = true

[commit]
    gpgSign = true
    verbose = true

[diff]
    algorithm = histogram
    colormoved = dimmed-zebra
    colormovedws = ignore-all-space

[merge]
    ff = false
    conflictstyle = diff3
    log = true

[gc]
    auto = 50
    pruneExpire = now
    # Reduce aggressiveness for musl systems
    aggressive = false

[pack]
    # Optimize for low-memory systems
    threads = 1
    deltaCacheSize = 64m
    windowMemory = 64m
    packSizeLimit = 100m

[fetch]
    prune = true
    parallel = 2
    writeCommitGraph = true

[maintenance]
    auto = true
    schedule = weekly
    task = gc,commit-graph,loose-objects,incremental-repack

[status]
    showUntrackedFiles = all
    submoduleSummary = true

[transfer]
    fsckObjects = true

[receive]
    fsckObjects = true
    denyDeletes = true
    denyNonFastForwards = true

[push]
    default = simple
    autoSetupRemote = true
    followTags = true

[alias]
    # Common operations
    co = checkout
    br = branch
    ci = commit
    st = status
    # Enhanced logging
    lg = log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset'
    # Development workflow
    unstage = reset HEAD --
    last = log -1 HEAD
    visual = !gitk
    # Safety aliases
    pushf = push --force-with-lease
    # Code review helpers
    files = !git diff --name-only $(git merge-base HEAD \"$REVIEW_BASE\")
    stat = !git diff --stat $(git merge-base HEAD \"$REVIEW_BASE\")

[url "ssh://git@github.com/"]
    insteadOf = https://github.com/
```

```sh
# Development
make ENV=dev            # Build
make dev-install       # Install
make dev-setup        # Setup configs
make clean-dev        # Clean

# Production
make ENV=prod          # Build
make package          # Create distribution
sudo make install-prod # Install
make clean-prod       # Clean
```


## Testing

```sh
make clean && make test
```


# Compile with debug symbols
make ENV=dev

# Run tests with GDB
gdb --args ./test_runner

# Common GDB commands
break test_mem_alloc    # Set breakpoint at specific function
break test_cache_lfu    # Set breakpoint at failing test
run                     # Start program
next                    # Step over
step                    # Step into
print ptr              # Print variable value
bt                     # Show backtrace

# Check for memory leaks and errors
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./test_runner

# More detailed memory access tracking
valgrind --tool=memcheck \
         --leak-check=full \
         --track-origins=yes \
         ./test_runner


## **2. Networking Modules**
Networking modules act as the "I/O system" of your web application.

### **Socket Communication**
- **Purpose**: Handle low-level socket operations.
- **Components**:
  - `net.c`: Create, bind, and listen on sockets.
  - `tcp.c`: Handle TCP connections (blocking/non-blocking).
  - `udp.c`: Handle UDP datagrams (if required).
  - `index.html`: Sample HTML file for testing.
- **Order of Development**:
  1. `net.c` for socket setup and teardown.
  2. `tcp.c` for request/response handling.
  3. `udp.c` if needed for stateless communication.
  4. `index.html` for testing static file serving.

---

### **2.2. HTTP Protocol**
- **Purpose**: Parse HTTP requests and construct responses.
- **Components**:
  - `http_parser.c`: Parse HTTP requests into structured data.
  - `http_response.c`: Generate HTTP responses.
  - `mime.c`: Map file extensions to MIME types.
- **Order of Development**:
  1. `http_parser.c` for request parsing.
  2. `http_response.c` for response generation.
  3. `mime.c` for content type mapping.

---

### **2.3. Routing and Request Handling**
- **Purpose**: Route HTTP requests to the correct handler.
- **Components**:
  - `router.c`: Match URLs to handler functions.
  - `middleware.c`: Implement middleware for preprocessing requests.
  - `handler.c`: Define handlers for specific routes.
- **Order of Development**:
  1. `router.c` for basic URL matching.
  2. `middleware.c` for logging, authentication, etc.
  3. `handler.c` for route-specific logic.

---

## **3. Data Management Modules**
Data management modules serve as the "filesystem" of your web application.

### **3.1. Database Integration**
- **Purpose**: Handle persistent storage using a flat-file database or SQLite.
- **Components**:
  - `db.c`: Manage database connections and queries.
  - `schema.c`: Define the database schema.
- **Order of Development**:
  1. `db.c` for database operations.
  2. `schema.c` for defining tables and relations.

---

### **3.2. Session Management**
- **Purpose**: Handle user sessions and authentication.
- **Components**:
  - `session.c`: Manage session tokens and expiration.
  - `auth.c`: Implement user authentication and authorization.
- **Order of Development**:
  1. `session.c` for token-based session management.
  2. `auth.c` for user authentication logic.

---

### **3.3. Static File Serving**
- **Purpose**: Serve static files (HTML, CSS, JS) efficiently.
- **Components**:
  - `static.c`: Locate and serve files from a `static/` directory.
  - `mime.c`: Reuse MIME mapping for file types.
- **Order of Development**:
  1. `static.c` for file reading and response generation.

---

## **4. Utility Modules**
Utility modules act as the "standard library" of your web application.

### **4.1. String Utilities**
- **Purpose**: Handle common string operations.
- **Components**:
  - `string_utils.c`: Implement functions for splitting, trimming, etc.

### **4.2. Error Handling**
- **Purpose**: Provide consistent error handling and reporting.
- **Components**:
  - `error.c`: Centralize error codes and messages.

---

## **5. Environment Variables and Configuration**
Environment variables and configuration files allow dynamic customization.

### **Configuration File (`config.conf`)**
```conf
SERVER_PORT=8080
DB_PATH=data/database.db
LOG_LEVEL=INFO
STATIC_DIR=static/
```

### **Environment Variable Access**
- Use `getenv` and `setenv` to access environment variables.
- Implement a wrapper function for fallback to default values.

**Example: `config.c`**
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

const char *get_env(const char *key, const char *default_value) {
    const char *value = getenv(key);
    return value ? value : default_value;
}

void load_config(AppConfig *config) {
    config->server_port = atoi(get_env("SERVER_PORT", "8080"));
    strcpy(config->db_path, get_env("DB_PATH", "data/database.db"));
    strcpy(config->log_level, get_env("LOG_LEVEL", "INFO"));
    strcpy(config->static_dir, get_env("STATIC_DIR", "static/"));
}
```

**Header: `config.h`**
```c
#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int server_port;
    char db_path[256];
    char log_level[16];
    char static_dir[256];
} AppConfig;

void load_config(AppConfig *config);

#endif
```

---

## **6. Build and Deployment**
### **Makefile**
Automate building the application with environment-specific configurations.
```makefile
CC = musl-gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread

SRC = src/main.c src/http_server.c src/router.c src/util.c src/db.c src/config.c
OBJ = $(SRC:.c=.o)
TARGET = webapp

all: $(TARGET)

$(TARGET): $(OBJ)
    $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
    rm -f $(OBJ) $(TARGET)
```

---

## **Development Order**
1. **Core Modules**: `config.c`, `env.c`, `logging.c`.
2. **Networking**: `net.c`, `tcp.c`, `http_parser.c`.
3. **Routing and Request Handling**: `router.c`, `handler.c`.
4. **Data Management**: `db.c`, `session.c`, `auth.c`.
5. **Static File Serving**: `static.c`, `mime.c`.
6. **Utility Modules**: `string_utils.c`, `error.c`.

This approach ensures a logical progression from foundational to application-specific functionality. Would you like implementation details for any specific module?
