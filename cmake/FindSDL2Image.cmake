set(LOCAL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL2_image/x86_64-w64-mingw32)

find_library(SDL_IMAGE_LIBRARY NAME SDL2_image
        HINTS
        /usr/lib/x86_64-linux-gnu
        ${LOCAL_PREFIX}/lib
        REQUIRED)

add_library(SDL2Image_ INTERFACE)

if(UNIX)
    target_link_libraries(SDL2Image_ INTERFACE ${SDL_IMAGE_LIBRARY} -lpng -ljpeg -ltiff -lwebp)
else()
    target_include_directories(SDL2Image_ INTERFACE ${LOCAL_PREFIX}/include/SDL2)
    target_link_libraries(SDL2Image_ INTERFACE ${SDL_IMAGE_LIBRARY})
endif()

add_library(SDL2::Image ALIAS SDL2Image_)