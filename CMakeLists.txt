cmake_minimum_required(VERSION 3.20)
project(p5 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)

add_executable(p5
    src/main.cpp
    src/lib.cpp
    src/Particle.cpp
    src/Simulator.cpp
    src/DataManager.cpp

    include/lib.hpp
    include/Particle.hpp
    include/Obstacle.hpp
    include/Simulator.hpp
    include/DataManager.hpp
)

target_include_directories(p5 PRIVATE include)

# 3. Vincular el módulo Gui de Qt a tu ejecutable
target_link_libraries(p5 PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
)
