cmake_minimum_required(VERSION 3.16)

project(QtPolymorphismDemo LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- FIND Qt6 MODULES ---
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- ADD EXECUTABLE ---
add_executable(QtPolymorphismDemo
    main.cpp
)

# --- LINK LIBRARIES ---
target_link_libraries(QtPolymorphismDemo
    Qt${QT_VERSION_MAJOR}::Widgets
)

# --- OPTIONAL INSTALL ---
include(GNUInstallDirs)
install(TARGETS QtPolymorphismDemo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

