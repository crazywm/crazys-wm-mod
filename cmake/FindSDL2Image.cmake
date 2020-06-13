if(NOT TARGET SDL2::SDL2)
    find_package(SDL2 REQUIRED)
endif()

set(LOCAL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}../deps/SDL2_image/x86_64-w64-mingw32)

find_library(SDL_IMAGE_LIBRARY NAME SDL2_image
        HINTS
        /usr/lib/x86_64-linux-gnu
        ${LOCAL_PREFIX}/lib
        REQUIRED)

add_library(SDL2Image_ INTERFACE)

if(UNIX)
    target_link_libraries(SDL2Image_ INTERFACE ${SDL_IMAGE_LIBRARY} SDL2::SDL2 -lpng -ljpeg -ltiff -lwebp)
else()
    target_link_libraries(SDL2Image_ INTERFACE ${LOCAL_PREFIX}/include/SDL2)
endif()

add_library(SDL2::Image ALIAS SDL2Image_)