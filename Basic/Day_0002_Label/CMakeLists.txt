cmake_minimum_required(VERSION 3.16)
project(MyLabelLibrary)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_standard_project_setup()

# Add both our source files
qt6_add_executable(MyLabelLibrary main.cpp label_update.cpp)

target_link_libraries(MyLabelLibrary
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)



