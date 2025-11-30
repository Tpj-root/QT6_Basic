cmake_minimum_required(VERSION 3.16)
project(day1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core LinguistTools)

# Source files
set(SOURCES
    main.cpp
)

# Translation files
set(TRANSLATIONS
    day1_ta_IN.qm
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} Qt6::Core)

# Copy .qm files to build folder
foreach(qm_file ${TRANSLATIONS})
    configure_file(${CMAKE_SOURCE_DIR}/${qm_file} ${CMAKE_CURRENT_BINARY_DIR}/${qm_file} COPYONLY)
endforeach()

