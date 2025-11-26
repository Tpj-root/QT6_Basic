cmake_minimum_required(VERSION 3.16)
project(HelloWorld)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_standard_project_setup()

qt6_add_executable(HelloWorld main.cpp)

target_link_libraries(HelloWorld
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)

