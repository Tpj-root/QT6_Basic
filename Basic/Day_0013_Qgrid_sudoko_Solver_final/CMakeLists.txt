cmake_minimum_required(VERSION 3.16)
project(Calculate)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

qt6_standard_project_setup()

# Copy gcode_gen.out to build directory
configure_file(
    "${CMAKE_SOURCE_DIR}/gcode_gen.out"
    "${CMAKE_BINARY_DIR}/gcode_gen.out"
    COPYONLY
)

# Add all our source files
qt6_add_executable(Calculate 
    main.cpp 
    sudokusolver.cpp
)

target_link_libraries(Calculate
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)

# Make sure gcode_gen.out is executable - FIXED TARGET NAME
add_custom_command(TARGET Calculate POST_BUILD
    COMMAND chmod +x "${CMAKE_BINARY_DIR}/gcode_gen.out"
    COMMENT "Making gcode_gen.out executable"
)
