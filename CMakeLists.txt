cmake_minimum_required(VERSION 3.16)
project(pulsar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(pulsar
    src/main.cpp
    src/main_window.cpp
    src/title_bar.cpp
    src/cosmic_background.cpp
    src/markdown_highlighter.cpp
    src/markdown_editor.cpp
    src/file_tree.cpp
)
target_include_directories(pulsar PRIVATE src)
target_link_libraries(pulsar PRIVATE Qt6::Widgets)