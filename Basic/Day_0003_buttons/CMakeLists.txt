cmake_minimum_required(VERSION 3.16)
project(Myawesome_button)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_standard_project_setup()

# Add all our source files
qt6_add_executable(Myawesome_button 
    main.cpp 
    label_update.cpp 
    awesome_button.cpp
)


target_link_libraries(Myawesome_button
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)



