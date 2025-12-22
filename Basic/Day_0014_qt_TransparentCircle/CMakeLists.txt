cmake_minimum_required(VERSION 3.16)
project(TransparentCircle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(SOURCES
    main.cpp
    mainwindow.cpp
)

set(HEADERS
    mainwindow.h
)

qt_wrap_cpp(MOC_SOURCES ${HEADERS})  # Run moc on headers

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${MOC_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# Enable automatic moc
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)
