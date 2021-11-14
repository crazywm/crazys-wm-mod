cmake_minimum_required(VERSION 3.12)
project(WM VERSION 7.1.2.4)
set(PROJECT_VERSION_TYPE alpha)

# this needs to happen before the find_package calls, because they register the dlls
# for install under windows
if(UNIX)
else()
    # on windows, don't put the dlls/exes into their own directory
    set(CMAKE_INSTALL_BINDIR .)
    # and find the required mingw dlls in its bin dir; add them to installation
    if( MINGW )
        message( STATUS "    Searching for MinGW DLLs." )
        get_filename_component( Mingw_Path ${CMAKE_CXX_COMPILER} PATH )
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS
                ${Mingw_Path}/libgcc_s_seh-1.dll
                ${Mingw_Path}/libstdc++-6.dll
                ${Mingw_Path}/libwinpthread-1.dll)
        message( STATUS "    Found ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}" )
    endif( MINGW )
    include( InstallRequiredSystemLibraries )
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
        Resources/Themes
        Resources/Data
        Resources/Interface
        Resources/Items
        Resources/Scripts
        Resources/Images
        DESTINATION Resources)

# make sure there is a Saves directory
install(DIRECTORY DESTINATION Saves)

install(FILES Resources/Characters/randoms.rgirlsx DESTINATION Resources/Characters)
if(NOT ${PROJECT_VERSION_TYPE} STREQUAL "alpha")
    install(DIRECTORY Resources/DefaultImages DESTINATION Resources)
endif()
install(FILES README.md CHANGES.md "Special Thanks.txt" gpl.txt config.xml segoeui.ttf DESTINATION .)
install(TARGETS WhoreMaster lua tinyxml2)

#  install the system packages
if( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS )
    install( PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin COMPONENT System )
endif( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS )

set(CPACK_GENERATOR "ZIP")
include(CPack)
