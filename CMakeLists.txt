cmake_minimum_required(VERSION 3.12)
project(WM)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(SDL2TTF REQUIRED)

find_package(Doxygen
  COMPONENTS dot
  OPTIONAL_COMPONENTS mscgen dia)

if(DOXYGEN_FOUND)

  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_SOURCE_BROWSER YES)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_EXTRACT_PACKAGE YES)
  set(DOXYGEN_EXTRACT_STATIC YES)
  set(DOXYGEN_EXTRACT_ANON_NSPACES YES)
  set(DOXYGEN_INTERNAL_DOCS YES)
  set(DOXYGEN_REFERENCED_BY_RELATION YES)
  set(DOXYGEN_REFERENCES_RELATION YES)
  set(DOXYGEN_GENERATE_TREEVIEW YES)
  set(DOXYGEN_CALL_GRAPH YES)
  set(DOXYGEN_CALLER_GRAPH YES)
  set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

  set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/docs")
  set(DOXYGEN_EXCLUDE_PATTERNS
    */deps/*
    )
  doxygen_add_docs(
    doxygen
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate HTML docs"
    )
endif(DOXYGEN_FOUND)

add_subdirectory(deps/tinyxml EXCLUDE_FROM_ALL)
add_subdirectory(deps/doctest EXCLUDE_FROM_ALL)
add_subdirectory(deps/lua EXCLUDE_FROM_ALL)
add_subdirectory(src/engine EXCLUDE_FROM_ALL)
add_subdirectory(src/game)
