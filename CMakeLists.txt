cmake_minimum_required(VERSION 3.20)

# Project Name and Settings
project(EnvEngWebApp C)

# Set the C Standard and Compiler
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_COMPILER musl-gcc)
set(CMAKE_LINKER /usr/local/bin/musl-linker)

# Enable position-independent code (PIC)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Compiler Flags
add_compile_options(
    -D_POSIX_C_SOURCE=200809L
    -D_XOPEN_SOURCE=800
    -O3
    -Wall
    -Wextra
    -pedantic
    -I/usr/include/x86_64-linux-musl
    -I${CMAKE_SOURCE_DIR}/include
    -L/usr/lib/x86_64-linux-musl
    -static
    -fno-common
    -ffunction-sections
    -fdata-sections
    -march=x86-64
    -Wshadow
    -Wconversion
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wold-style-definition
    -Wmissing-declarations
    -Wredundant-decls
    -Wfloat-equal
    -Wundef
    -Wbad-function-cast
    -Wcast-qual
    -Wwrite-strings
    -Wformat=2
    -Wstrict-aliasing=2
    -Wstrict-overflow=5
    -Wnull-dereference
    -Wdouble-promotion
    -Wvla
    -Wcast-align
    -Wpointer-arith
    -Wswitch-default
    -Wunreachable-code
)

# Include Directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Source Files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

# Create Executable
add_executable(enveng_web_app ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(enveng_web_app PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(enveng_web_app PRIVATE pthread m)

# Linker Flags
set_target_properties(enveng_web_app PROPERTIES
    LINK_FLAGS "-fuse-ld=/usr/bin/musl-ldd -static"
)

# Ensure Ninja is used as the generator
if (NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL_ERROR "Please use Ninja as the generator: cmake -G Ninja .")
endif()

# Set static linking flags
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)

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
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY};--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy;--; -Iinclude; -fix-errors")
endif()

# Unit Tests (Optional Integration)
enable_testing()
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*.c)
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# Doxygen Integration (Documentation)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/docs)

    add_custom_target(docs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Install Target
install(TARGETS enveng_web_app DESTINATION /usr/local/bin)
install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION /usr/share/doc/enveng_web_app)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs DESTINATION /usr/share/doc/enveng_web_app)

# Packaging
include(CPack)
