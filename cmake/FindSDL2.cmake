if(UNIX)
    # here, we expect to have SDL2 be provided by the package manager
    # we need to temporarily clear the CMAKE_MODULE_PATH to get access to the system package instead of
    # recursing into this script
    set(SAVE_MP ${CMAKE_MODULE_PATH})
    unset(CMAKE_MODULE_PATH)
    find_package(SDL2 REQUIRED)
    set(CMAKE_MODULE_PATH  ${SAVE_MP})


    # apparently, this will not generate the SDL2::SDL2 target
    add_library(SDL2_ INTERFACE)
    target_link_libraries(SDL2_ INTERFACE ${SDL2_LIBRARIES})
    target_include_directories(SDL2_ INTERFACE ${SDL2_INCLUDE_DIRS})

    add_library(SDL2::SDL2 ALIAS SDL2_)
else()
    # this is copied and adapted from the cmake config files provided by SDL
    set(prefix ${CMAKE_CURRENT_SOURCE_DIR}/../deps)
    set(exec_prefix "${prefix}")
    set(libdir "${exec_prefix}/lib")
    set(SDL2_PREFIX "SDL2/x86_64-w64-mingw32")
    set(SDL2_EXEC_PREFIX "SDL2/x86_64-w64-mingw32")
    set(SDL2_LIBDIR "${exec_prefix}/lib")
    set(SDL2_INCLUDE_DIRS "${prefix}/include/SDL2")
    set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lmingw32 -lSDL2main -lSDL2 -mwindows")
    string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

    if(NOT TARGET SDL2::SDL2)
        # Remove -lSDL2 as that is handled by CMake, note the space at the end so it does not replace e.g. -lSDL2main
        # This may require "libdir" beeing set (from above)
        string(REPLACE "-lSDL2 " "" SDL2_EXTRA_LINK_FLAGS " -lmingw32 -lSDL2main -lSDL2 -mwindows ")
        string(STRIP "${SDL2_EXTRA_LINK_FLAGS}" SDL2_EXTRA_LINK_FLAGS)
        string(REPLACE "-lSDL2 " "" SDL2_EXTRA_LINK_FLAGS_STATIC " -lmingw32 -lSDL2main -lSDL2 -mwindows  -Wl,--no-undefined -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid -static-libgcc ")
        string(STRIP "${SDL2_EXTRA_LINK_FLAGS_STATIC}" SDL2_EXTRA_LINK_FLAGS_STATIC)

        add_library(SDL2::SDL2 SHARED IMPORTED)
        set_target_properties(SDL2::SDL2 PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${prefix}/include/SDL2"
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                IMPORTED_LOCATION "${exec_prefix}/lib/libSDL2.so"
                INTERFACE_LINK_LIBRARIES "${SDL2_EXTRA_LINK_FLAGS}")

        add_library(SDL2::SDL2-static STATIC IMPORTED)
        set_target_properties(SDL2::SDL2-static PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${prefix}/include/SDL2"
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                IMPORTED_LOCATION "${exec_prefix}/lib/libSDL2.a"
                INTERFACE_LINK_LIBRARIES "${SDL2_EXTRA_LINK_FLAGS_STATIC}")

        add_library(SDL2::SDL2main STATIC IMPORTED)
        set_target_properties(SDL2::SDL2main PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                IMPORTED_LOCATION "${exec_prefix}/lib/libSDL2main.a")
    endif()

endif()