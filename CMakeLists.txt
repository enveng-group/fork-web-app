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
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/config_loader.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/constants.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/csv_utils.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/data_structures.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/env_loader.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/error_codes.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/error_handler.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/garbage_collector.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/logger.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/records.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/rec_utils.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/test_framework.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/utils.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* include/validator.h -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/config_loader.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/constants.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/csv_utils.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/data_structures.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/env_loader.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/error_handler.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/garbage_collector.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/logger.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/main.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/records.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/rec_utils.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/utils.c -- build
        COMMAND ${CLANG_TIDY} -fix -header-filter=.* src/validator.c -- build
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy on source and header files"
    )
endif()

# Add cppcheck
find_program(CPPCHECK "cppcheck")
if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK} --enable=all --inconclusive --std=c90 --platform=unix64 --suppress=missingIncludeSystem -I${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running cppcheck on source files"
    )
endif()

# Add Doxygen
find_package(Doxygen REQUIRED)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc_doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif(DOXYGEN_FOUND)

# Add custom target to build all
add_custom_target(build_all_custom
    DEPENDS format clang_tidy cppcheck doc_doxygen
)

# Ensure format, clang-tidy, cppcheck, and Doxygen run before building
add_dependencies(build_all_custom format clang_tidy cppcheck doc_doxygen)

# Add main build target
add_custom_target(run_all_tools
    COMMAND ${CMAKE_COMMAND} --build .
    COMMENT "Running all tools"
)

# Add an option to enable or disable tests
option(ENABLE_TESTS "Enable tests" OFF)

if(ENABLE_TESTS)
    file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*.c)
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()

    # Define the execute_tests target
    add_custom_target(execute_tests
        COMMAND ${CMAKE_CTEST_COMMAND}
        DEPENDS ${TEST_SOURCES}
        COMMENT "Running all tests"
    )

    # Add custom target to run tests
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND}
        DEPENDS enveng_web_app
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running all tests"
    )
endif()

# Install Target
install(TARGETS enveng_web_app DESTINATION /usr/local/bin)
install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION /usr/share/doc/enveng_web_app)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs DESTINATION /usr/share/doc/enveng_web_app)

# Packaging
include(CPack)
