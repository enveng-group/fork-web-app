cmake_minimum_required(VERSION 3.10)

# Project Name and Settings
project(EnvEngWebApp C)

# Set the C Standard and Compiler
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_COMPILER musl-clang)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c90 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -I/usr/local/include -I/usr/local/include/openssl -Wall -ansi -Wextra -pedantic -O3 -ffunction-sections -fdata-sections -fno-common -fstack-protector-strong -fPIC -Wno-long-long -g")
set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/lib -static")

# Disable C++ compiler
set(CMAKE_CXX_COMPILER "")

# Enable position-independent code (PIC)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include Directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    /usr/local/include
    /usr/local/include/openssl
    /usr/local/include/linux_header
    include
)

# Add library directories
link_directories(
    /usr/local/lib
    /usr/local/lib/ssl
)

# Set static linking flags
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

# Add source files
set(SOURCE_FILES
    src/add_custom_uint64.s
    src/atomic_ops.c
    src/compat.c
    src/compat_layer.c
    src/compatStrtol64_asm.s
    src/config_loader.c
    src/constants.c
    src/csv_utils.c
    src/data_structures.c
    src/div_custom_uint64.s
    src/env_loader.c
    src/error_handler.c
    src/garbage_collector.c
    src/http_parser.c
    src/http_response.c
    src/logger.c
    src/main.c
    src/records.c
    src/rec_utils.c
    src/server.c
    src/sha256_asm.c
    src/socket_module.c
    src/ssl_module.c
    src/static_file_handler.c
    src/utils.c
    src/validator.c
)

# Add executable target
add_executable(enveng_web_app ${SOURCE_FILES})

# Find OpenSSL library
find_package(OpenSSL REQUIRED)

# Include OpenSSL directories
include_directories(${OPENSSL_INCLUDE_DIR})

# Link OpenSSL libraries
target_link_libraries(enveng_web_app ${OPENSSL_LIBRARIES} -ldl)

# Link additional libraries and source files
target_link_libraries(enveng_web_app
    /usr/local/lib64/libcrypto.a
    /usr/local/lib64/libssl.a
    -ldl
    -static
)

# Add source files containing the missing functions
target_sources(enveng_web_app PRIVATE
    ${CMAKE_SOURCE_DIR}/src/compat.c
    ${CMAKE_SOURCE_DIR}/src/compat_layer.c
    ${CMAKE_SOURCE_DIR}/src/data_structures.c
    ${CMAKE_SOURCE_DIR}/src/http_parser.c
    ${CMAKE_SOURCE_DIR}/src/records.c
    ${CMAKE_SOURCE_DIR}/src/static_file_handler.c
    ${CMAKE_SOURCE_DIR}/src/add_custom_uint64.s
    ${CMAKE_SOURCE_DIR}/src/div_custom_uint64.s
    ${CMAKE_SOURCE_DIR}/src/compatStrtol64_asm.s
)

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
        # Add other header files as needed
    )
endif()

# Add cppcheck
find_program(CPPCHECK "cppcheck")
if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK} --enable=all --inconclusive --quiet src include
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running cppcheck on source and header files"
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
install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION /usr/share/doc/web_app)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs DESTINATION /usr/share/doc/web_app)

# Packaging
include(CPack)
