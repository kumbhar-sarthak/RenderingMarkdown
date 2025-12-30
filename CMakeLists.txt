cmake_minimum_required(VERSION 3.20)
project(MarkdownParser VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/definition/heading_rule.cc
    src/definition/code_rule.cc
    src/definition/bold_rule.cc
    src/definition/italic_rule.cc
    src/definition/link_rule.cc
    src/definition/list_rule.cc
    src/definition/horizontalline_rule.cc
    src/lexer.cpp
    src/parser.cpp
    src/main.cpp
)

add_executable(MarkdownEngine ${SOURCES})

target_include_directories(MarkdownEngine PRIVATE includes includes/rules)

find_package(Threads REQUIRED)
target_link_libraries(MarkdownEngine PRIVATE Threads::Threads)