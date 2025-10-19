cmake_minimum_required(VERSION 3.26)
project(TESTCompiler)

# 设置C++标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 检测平台和编译器
if(WIN32)
    # Windows平台使用MSVC编译器
    if(NOT MSVC)
        message(WARNING "Windows平台建议使用MSVC编译器")
    endif()
    # 当前使用Ninja生成器，将来会替换为Visual Studio生成器
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        message(STATUS "Windows平台当前使用 ${CMAKE_GENERATOR} 生成器，建议使用Ninja")
    endif()
else()
    # Linux平台使用GCC编译器
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(WARNING "Linux平台建议使用GCC编译器")
    endif()
    # Linux平台使用Makefile生成器
    if(NOT CMAKE_GENERATOR STREQUAL "Unix Makefiles")
        message(STATUS "Linux平台当前使用 ${CMAKE_GENERATOR} 生成器，建议使用Unix Makefiles")
    endif()
endif()

# 设置编译选项
if(MSVC)
    # MSVC编译器选项
    add_compile_options(/W3)
    add_compile_options(/utf-8)
else()
    # GCC编译器选项
    add_compile_options(-Wall -Wextra)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/Debug/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/Release/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/Debug/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/Release/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/Debug/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/Release/lib)

# 为所有配置设置输出目录
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/build/${OUTPUTCONFIG}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/build/${OUTPUTCONFIG}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/build/${OUTPUTCONFIG}/lib)
endforeach()

# 包含目录
include_directories(include)
include_directories(build/generated)

# 编译器主程序源文件
file(GLOB_RECURSE COMPILER_SOURCES 
    src/AST/*.cpp
    src/Lexer/*.cpp
    src/Parser/*.cpp
    src/*.cpp
    src/*.cc
    src/*.cxx
)

# 编译器主程序头文件
file(GLOB_RECURSE COMPILER_HEADERS
    include/*.hpp
    include/*.h
    build/generated/*.hpp
    build/generated/*.h
)

# 创建主可执行文件，包含所有源文件
add_executable(TESTCompiler ${COMPILER_SOURCES})
target_include_directories(TESTCompiler PRIVATE include build/generated)

# 添加宏定义
target_compile_definitions(TESTCompiler PRIVATE LEXER_ENABLED)
target_compile_definitions(TESTCompiler PRIVATE PARSER_ENABLED)

# 选项：是否编译工具
option(BUILD_TOOLS "Build DFA Generator and Parser Generator tools" ON)
option(BUILD_DFA_GENERATOR "Build DFA Generator tool" ON)
option(BUILD_PARSER_GENERATOR "Build Parser Generator tool" ON)

# 添加工具子项目
if(BUILD_TOOLS OR BUILD_DFA_GENERATOR)
    add_subdirectory("Tools/DFA-Generator")
endif()

if(BUILD_TOOLS OR BUILD_PARSER_GENERATOR)
    add_subdirectory("Tools/Parser-Generator")
endif()

# 选项：是否编译测试（默认关闭）
option(BUILD_TESTS "Build test programs" OFF)

if(BUILD_TESTS)
    enable_testing()
    
    # 词法分析器测试
    file(GLOB_RECURSE LEXER_TEST_SOURCES tests/lexer/*.cpp)
    if(LEXER_TEST_SOURCES)
        # 将词法分析器源文件添加到测试中（排除main.cpp）
        file(GLOB_RECURSE TEST_COMPILER_SOURCES 
            src/Lexer/*.cpp
            src/AST/*.cpp
            src/Parser/*.cpp
        )
        add_executable(LexerTest ${LEXER_TEST_SOURCES} ${TEST_COMPILER_SOURCES})
        target_include_directories(LexerTest PRIVATE include build/generated)
        add_test(NAME LexerTest COMMAND LexerTest)
    endif()
endif()

# 创建必要的目录
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/Debug/bin)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/Debug/lib)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/Release/bin)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/Release/lib)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/generated)

# 打印配置信息
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build Tools: ${BUILD_TOOLS}")
message(STATUS "Build DFA Generator: ${BUILD_DFA_GENERATOR}")
message(STATUS "Build Parser Generator: ${BUILD_PARSER_GENERATOR}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
