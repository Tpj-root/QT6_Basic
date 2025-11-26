cmake_minimum_required(VERSION 3.16)
project(ToolbarDemo)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_standard_project_setup()

# Add all our source files
qt6_add_executable(ToolbarDemo 
    main.cpp 
    mainwindow.cpp
)


target_link_libraries(ToolbarDemo
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)



