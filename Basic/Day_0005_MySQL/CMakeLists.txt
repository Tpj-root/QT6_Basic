cmake_minimum_required(VERSION 3.16)
project(productmanager)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

qt6_standard_project_setup()

# Add all our source files
qt6_add_executable(productmanager 
    main.cpp 
    productmanager.cpp
)


target_link_libraries(productmanager
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
)



