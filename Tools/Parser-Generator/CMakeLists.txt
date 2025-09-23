cmake_minimum_required(VERSION 3.26)

# 如果这个文件作为子项目被包含，则不需要重新设置项目
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    project(Parser_Generator)
endif()

# 设置输出目录为Tools目录下的相应位置
set(PARSER_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(PARSER_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)
set(PARSER_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)
set(PARSER_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)

# 为所有配置设置输出目录
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUTCONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUTCONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/bin/${OUTPUTCONFIG})
endforeach()

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/header)

# 收集源文件
file(GLOB_RECURSE PARSER_GENERATOR_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cxx
)

# 收集头文件
file(GLOB_RECURSE PARSER_GENERATOR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/header/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/header/*.h
)

# 创建Parser生成器可执行文件
if(PARSER_GENERATOR_SOURCES)
    add_executable(ParserGenerator ${PARSER_GENERATOR_SOURCES})
    target_include_directories(ParserGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header)
    
    # 设置可执行文件的输出目录
    set_target_properties(ParserGenerator PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PARSER_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PARSER_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
    )
else()
    # 如果没有源文件，创建一个占位的可执行文件
    message(STATUS "No source files found for Parser Generator, creating placeholder")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/parser_generator_main.cpp 
"#include <iostream>

int main() {
    std::cout << \"Parser Generator placeholder - implementation pending\" << std::endl;
    return 0;
}
")
    add_executable(ParserGenerator ${CMAKE_CURRENT_BINARY_DIR}/parser_generator_main.cpp)
    target_include_directories(ParserGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header)
    
    # 设置可执行文件的输出目录
    set_target_properties(ParserGenerator PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PARSER_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PARSER_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
    )
endif()

# 创建必要的目录
file(MAKE_DIRECTORY ${PARSER_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
file(MAKE_DIRECTORY ${PARSER_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

# 打印Parser生成器配置信息
message(STATUS "Parser Generator configured")
message(STATUS "Parser Generator sources: ${PARSER_GENERATOR_SOURCES}")
message(STATUS "Parser Generator headers: ${PARSER_GENERATOR_HEADERS}")