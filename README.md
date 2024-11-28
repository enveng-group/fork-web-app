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


To ensure **POSIX compliance** while integrating the **musl C library** and statically linking everything into your program, follow these instructions. musl is a lightweight, fast, and POSIX-compliant C standard library that can replace glibc for static linking.

---

## **1. Prerequisites**

Install and ensure the availability of the following **POSIX-compliant tools** and libraries:

- **Compiler**: `clang` (set to use musl as the C library)
- **Build System**: `make`
- **Linker**: `ld`
- **Static Analysis**: `lint` (e.g., `splint`)
- **Formatting**: `indent`
- **Debugger**: `gdb`
- **Profiling**: `gprof`
- **Memory Debugging**: `valgrind`
- **Documentation**: `man` or `groff`
- **Packaging**: `tar`, `gzip`, or `xz`
- **musl**: The musl C library

---

## **2. Installing musl**

You will need to install **musl** on your system or download a prebuilt version. To build and use musl with `clang`, follow these steps:

### **Install musl**

You can either download a precompiled musl package or build it from source. Here's how to build musl from source:

1. **Download musl source code**:
    ```sh
    wget https://musl.libc.org/releases/musl-1.2.3.tar.gz
    tar -xzf musl-1.2.3.tar.gz
    cd musl-1.2.3
    ```

2. **Configure and build musl**:
    ```sh
    ./configure --prefix=/usr/local/musl
    make
    sudo make install
    ```

### **Set Up musl with Clang**
After musl is installed, configure Clang to use musl as its standard C library by setting the `-target` and `-nostdlib` flags and linking against musl.

For **Clang**, use the following flags:
```sh
clang -target x86_64-linux-musl -nostdlib -L/usr/local/musl/lib -I/usr/local/musl/include -static <your_source_files> -o <output_file> -lm
```

### **Verify musl Installation**
To verify that musl is installed correctly and is being used:

```sh
/usr/local/musl/bin/musl-gcc --version
```

You can also check the runtime with the `file` command to ensure that musl is being used:
```sh
file ./my_program
```

---

## **3. Updated Directory Structure**

```sh
.
├── build/          # Build artifacts and temporary files.
├── bin/            # Compiled binary executables.
├── include/        # Header files.
├── lib/            # Static libraries.
├── src/            # Source files.
├── tests/          # Unit tests.
├── docs/           # Documentation (e.g., man pages).
└── tmp/            # Temporary files.
```

---

## **4. Updated Makefile Setup**

The **Makefile** is updated to use musl for static linking, with Clang configured to target musl.

#### **Root `Makefile`**

```makefile
# POSIX-compliant Makefile for building and testing with musl

CC = clang
CFLAGS = -std=c17 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -O0 -pedantic
LDFLAGS = -static -L/usr/local/musl/lib -I/usr/local/musl/include -lm
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests
INCLUDE_DIR = include
OBJ_FILES = $(BUILD_DIR)/main.o $(BUILD_DIR)/logger.o
TARGET = $(BIN_DIR)/my_program
TEST_TARGET = $(BIN_DIR)/test_runner

.PHONY: all clean test doc

all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJ_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Build and run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_DIR)/test_logger.o $(BUILD_DIR)/logger.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Generate documentation (man pages)
doc:
	groff -man docs/my_program.1 > $(BIN_DIR)/my_program.1.pdf
```

### **Important Points**:
- **`-static`**: Ensures the program is statically linked.
- **`-L/usr/local/musl/lib`** and **`-I/usr/local/musl/include`**: Directs Clang to use the musl library and include paths.
- **`-target x86_64-linux-musl`**: Specifies the musl target for Clang.

---

---

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