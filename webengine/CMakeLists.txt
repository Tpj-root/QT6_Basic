cmake_minimum_required(VERSION 3.16)
project(SudokuSolver)

find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(SudokuSolver
    main.cpp
)

target_link_libraries(SudokuSolver
    PRIVATE
        Qt6::Widgets
        Qt6::WebEngineWidgets
)

