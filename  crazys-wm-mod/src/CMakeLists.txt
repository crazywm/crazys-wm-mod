cmake_minimum_required(VERSION 3.10)
project(WM)

# make imported targets for SDL
find_package(SDL2 REQUIRED)
add_library(SDL2 INTERFACE)
target_link_libraries(SDL2 INTERFACE ${SDL2_LIBRARIES})
target_include_directories(SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})

find_library(SDL_IMAGE_LIBRARIES NAME SDL2_image HINTS /usr/lib/x86_64-linux-gnu REQUIRED)
add_library(SDL_image INTERFACE)
target_link_libraries(SDL_image INTERFACE ${SDL_IMAGE_LIBRARIES} -lpng -ljpeg -ltiff -lwebp)
# target_include_directories(SDL_image INTERFACE ${SDL_IMAGE_INCLUDE_DIRS})

add_library(SDL_ttf INTERFACE)
find_library(SDL_TTF_LIBRARIES NAME SDL2_ttf HINTS /usr/lib/x86_64-linux-gnu REQUIRED)
target_link_libraries(SDL_ttf INTERFACE ${SDL_TTF_LIBRARIES})
# target_include_directories(SDL_ttf INTERFACE ${SDL_TTF_INCLUDE_DIRS})

add_subdirectory(tinyxml)
add_subdirectory(lua)
add_subdirectory(engine)
add_subdirectory(game)
