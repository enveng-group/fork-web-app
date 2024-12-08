cmake_minimum_required(VERSION 3.20)

# Project Name and Settings
project(EnvEngWebApp C)

# Set the C Standard and Compiler
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_COMPILER musl-gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c90 -D_POSIX_C_SOURCE=199506L -D_XOPEN_SOURCE=500 -I/usr/include/x86_64-linux-musl -Wall -ansi -Wextra -pedantic -O3 -ffunction-sections -fdata-sections -fno-common -fstack-protector-strong -fPIC -static -g")

# Enable position-independent code (PIC)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include Directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Set static linking flags
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -static")

# Source Files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

# Create Executable
add_executable(enveng_web_app ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(enveng_web_app PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(enveng_web_app PRIVATE pthread m)

# Ensure Ninja is used as the generator
if (NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL_ERROR "Please use Ninja as the generator: cmake -G Ninja .")
endif()

# Output Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add a custom target for building the project
add_custom_target(build_all
    DEPENDS cppcheck
)

# Format source and header files
add_custom_target(format
    COMMAND clang-format -i src/*.c include/*.h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format on source and header files"
)

# Add clang-tidy checks
find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    add_custom_target(clang_tidy
        COMMAND clang-tidy -fix -header-filter=.* include/config_loader.h -- build
        COMMAND clang-tidy -fix -header-filter=.* include/constants.h -- build
        COMMAND clang-tidy -fix -header-filter=.* include/env_loader.h -- build
        COMMAND clang-tidy -fix -header-filter=.* include/error_handler.h -- build
        COMMAND clang-tidy -fix -header-filter=.* include/garbage_collector.h -- build
        COMMAND clang-tidy -fix -header-filter=.* include/logger.h -- build
        COMMAND clang-tidy -fix -header-filter=.* include/utils.h -- build
        COMMAND clang-tidy -fix -header-filter=.* include/validator.h -- build
        COMMAND clang-tidy -fix -header-filter=.* src/config_loader.c -- build
        COMMAND clang-tidy -fix -header-filter=.* src/constants.c -- build
        COMMAND clang-tidy -fix -header-filter=.* src/env_loader.c -- build
        COMMAND clang-tidy -fix -header-filter=.* src/error_handler.c -- build
        COMMAND clang-tidy -fix -header-filter=.* src/garbage_collector.c -- build
        COMMAND clang-tidy -fix -header-filter=.* src/logger.c -- build
        COMMAND clang-tidy -fix -header-filter=.* src/main.c -- build
        COMMAND clang-tidy -fix -header-filter=.* src/utils.c -- build
        COMMAND clang-tidy -fix -header-filter=.* src/validator.c -- build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy on source and header files"
    )
endif()

# Define the source files
set(SOURCE_FILES
    src/garbage_collector.c
    src/logger.c
    src/main.c
    src/utils.c
    src/validator.c
)

# Add a custom target for cppcheck
add_custom_target(cppcheck
    COMMAND cppcheck --enable=all --std=c90 -I /usr/include/x86_64-linux-musl -I include --suppress=unusedFunction --force ${SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running cppcheck"
    VERBATIM
)

# Add a custom target for building the project
add_custom_target(build_all_custom
    DEPENDS cppcheck
)

# Ensure cppcheck runs before building
add_dependencies(build_all_custom cppcheck)

# Unit Tests (Optional Integration)
enable_testing()
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*.c)
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# Install Target
install(TARGETS enveng_web_app DESTINATION /usr/local/bin)
install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION /usr/share/doc/enveng_web_app)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs DESTINATION /usr/share/doc/enveng_web_app)

# Packaging
include(CPack)
