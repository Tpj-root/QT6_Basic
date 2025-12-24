cmake_minimum_required(VERSION 3.16)
project(TransparentCube)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL)

set(SOURCES
    main.cpp
    glwindow.cpp
)

set(HEADERS
    glwindow.h
)

qt_wrap_cpp(MOC_SOURCES ${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
