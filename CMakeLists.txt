cmake_minimum_required(VERSION 3.12)
project(WM VERSION 7.01.13)

# this needs to happen before the find_package calls, because they register the dlls
# for install under windows
if(UNIX)
else()
    # on windows, don't put the dlls/exes into their own directory
    set(CMAKE_INSTALL_BINDIR .)
    install(DIRECTORY deps/mingw/ DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(SDL2TTF REQUIRED)
find_package(FFMPEG REQUIRED)

include(Doxygen)

add_subdirectory(deps/tinyxml EXCLUDE_FROM_ALL)
add_subdirectory(deps/doctest EXCLUDE_FROM_ALL)
add_subdirectory(deps/lua EXCLUDE_FROM_ALL)
add_subdirectory(src/engine EXCLUDE_FROM_ALL)
add_subdirectory(src/game)

install(DIRECTORY
        Resources/Buttons
        Resources/Data
        Resources/Interface
        Resources/Items
        Resources/Scripts
        Resources/Images
        DESTINATION Resources)
install(FILES Resources/Characters/randoms.rgirlsx DESTINATION Resources/Characters)
install(FILES README.md CHANGES.md "Special Thanks.txt" gpl.txt config.xml segoeui.ttf DESTINATION .)
install(TARGETS WhoreMaster lua tinyxml2)
set(CPACK_GENERATOR "ZIP")
include(CPack)
