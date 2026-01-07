# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(StencilGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Download and include nlohmann/json
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# Add the library
add_library(stencil_generator
    stencil_generator.cpp
)

target_include_directories(stencil_generator
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${nlohmann_json_SOURCE_DIR}/include
)

target_link_libraries(stencil_generator
    PUBLIC
    ${OpenCV_LIBS}
)

# Add example executable
add_executable(stencil_example
    examples/stencil_example.cpp
)

target_link_libraries(stencil_example
    stencil_generator
)

# Optional: Add tests
if(BUILD_TESTS)
    enable_testing()
    add_executable(stencil_tests
        tests/stencil_tests.cpp
    )
    target_link_libraries(stencil_tests
        stencil_generator
    )
    add_test(NAME stencil_tests COMMAND stencil_tests)
endif()