# Qt6 Stencil Generator CMake Configuration
cmake_minimum_required(VERSION 3.16)
project(QtStencilGenerator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)
find_package(OpenCV REQUIRED)

# Create executable
add_executable(QtStencilGenerator
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.hpp
    src/ImageViewer.cpp
    src/ImageViewer.hpp
    src/StencilGenerator.cpp
    src/StencilGenerator.hpp
    src/ProcessingWidget.cpp
    src/ProcessingWidget.hpp
    src/resources/icons.qrc
)

# Include directories
target_include_directories(QtStencilGenerator PRIVATE src)

# Link libraries
target_link_libraries(QtStencilGenerator
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGLWidgets
    ${OpenCV_LIBS}
)

# Set application properties on Windows
if(WIN32)
    set_target_properties(QtStencilGenerator PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()