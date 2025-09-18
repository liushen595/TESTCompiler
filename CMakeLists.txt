cmake_minimum_required(VERSION 3.16)
project(TESTCompiler LANGUAGES CXX)

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files (adjust names if your files are in subfolders)
set(SOURCES
    TESTmain.cpp
    TESTscan.cpp
    TESTparse.cpp
    TESTscan.hpp
    TESTparse.hpp
)

add_executable(TESTCompiler ${SOURCES})

# Compiler-specific flags
if(MSVC)
    # Use MSVC, enable UTF-8 source encoding and C++23 mode
    target_compile_options(TESTCompiler PRIVATE /utf-8 /W4 /EHsc)
else()
    # For GCC/Clang: try to keep UTF-8 input/output and use C++23
    target_compile_options(TESTCompiler PRIVATE -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

target_compile_features(TESTCompiler PRIVATE cxx_std_23)

# Place built exe into separate folders per configuration (debug/release)
# Debug outputs -> <project_root>/debug, Release outputs -> <project_root>/release
set_target_properties(TESTCompiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/debug/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/release/bin"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/debug/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/release/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/debug/bin"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/release/bin"
)

message(STATUS "Project: ${PROJECT_NAME}")
if(MSVC)
    message(STATUS "Configuring for MSVC: adding /utf-8 flags")
else()
    message(STATUS "Non-MSVC compiler: setting UTF-8 related flags (best-effort)")
endif()
