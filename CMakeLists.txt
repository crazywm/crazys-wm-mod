cmake_minimum_required(VERSION 3.12)
project(WM)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(SDL2TTF REQUIRED)

include(Doxygen)

add_subdirectory(deps/tinyxml EXCLUDE_FROM_ALL)
add_subdirectory(deps/doctest EXCLUDE_FROM_ALL)
add_subdirectory(deps/lua EXCLUDE_FROM_ALL)
add_subdirectory(src/engine EXCLUDE_FROM_ALL)
add_subdirectory(src/game)
