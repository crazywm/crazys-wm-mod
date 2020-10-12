cmake_minimum_required(VERSION 3.10)
project(WM)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(SDL2TTF REQUIRED)

add_subdirectory(deps/tinyxml)
add_subdirectory(deps/doctest)
add_subdirectory(deps/lua)
add_subdirectory(src/engine)
add_subdirectory(src/game)
