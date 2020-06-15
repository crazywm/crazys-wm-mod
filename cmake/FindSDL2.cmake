if(UNIX)
    # here, we expect to have SDL2 be provided by the package manager
    # we need to temporarily clear the CMAKE_MODULE_PATH to get access to the system package instead of
    # recursing into this script
    set(SAVE_MP ${CMAKE_MODULE_PATH})
    unset(CMAKE_MODULE_PATH)
    find_package(SDL2 REQUIRED)
    set(CMAKE_MODULE_PATH  ${SAVE_MP})
else()
    # this is copied and adapted from the cmake config files provided by SDL
    set(prefix ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2/x86_64-w64-mingw32)
    set(exec_prefix "${prefix}")
    set(libdir "${exec_prefix}/lib")
    set(SDL2_PREFIX "SDL2/x86_64-w64-mingw32")
    set(SDL2_EXEC_PREFIX "SDL2/x86_64-w64-mingw32")
    set(SDL2_LIBDIR "${exec_prefix}/lib")
    set(SDL2_INCLUDE_DIRS "${prefix}/include/SDL2")
    set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lmingw32 -lSDL2main -lSDL2 -mwindows")
    string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

endif()

add_library(SDL2_ INTERFACE)
target_link_libraries(SDL2_ INTERFACE ${SDL2_LIBRARIES})
target_include_directories(SDL2_ INTERFACE ${SDL2_INCLUDE_DIRS})
add_library(SDL2::SDL2 ALIAS SDL2_)
