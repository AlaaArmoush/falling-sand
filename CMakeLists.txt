cmake_minimum_required(VERSION 3.20)

project(falling_sand LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib 5.5 REQUIRED)

add_executable(falling_sand
    src/main.cpp
    src/platform/RaylibView.cpp
    src/platform/RaylibView.hpp
    src/simulation/Simulation.cpp
    src/simulation/Simulation.hpp
)

target_include_directories(falling_sand PRIVATE src)
target_compile_features(falling_sand PRIVATE cxx_std_20)

set_target_properties(falling_sand PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(falling_sand PRIVATE /W4 /permissive-)
else()
    target_compile_options(falling_sand PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(falling_sand PRIVATE raylib)

