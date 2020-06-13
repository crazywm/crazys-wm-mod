if(NOT TARGET SDL2::SDL2)
    find_package(SDL2 REQUIRED)
endif()

set(LOCAL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}../deps/SDL2_ttf/x86_64-w64-mingw32)

find_library(SDL2_TTF_LIBRARY NAME SDL2_ttf
        HINTS
        /usr/lib/x86_64-linux-gnu
        ${LOCAL_PREFIX}/lib
        REQUIRED)

add_library(SDL2TTF_ INTERFACE)

if(UNIX)
    target_link_libraries(SDL2TTF_ INTERFACE ${SDL2_TTF_LIBRARY} SDL2::SDL2)
else()
    target_link_libraries(SDL2TTF_ INTERFACE ${LOCAL_PREFIX}/include/SDL2)
endif()

add_library(SDL2::TTF ALIAS SDL2TTF_)