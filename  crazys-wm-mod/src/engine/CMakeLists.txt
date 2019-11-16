cmake_minimum_required(VERSION 3.10)
project(WM_Engine)

set(SRC CLog.cpp DirPath.cpp FileList.cpp sConfig.cpp SDL_anigif.c utils/cKeyValueStore.cpp)

set(INTERFACE_SRC
        interface/cColor.cpp
        interface/cAnimatedSurface.cpp
        interface/cFont.cpp
        interface/cInterfaceWindow.cpp
        interface/cSurface.cpp
        interface/cWindowManager.cpp
        interface/cChoiceMessage.cpp
        interface/CGraphics.cpp
        interface/cImageCache.cpp
        interface/cInterfaceWindowXML.cpp)

set(WIDGETS_SRC
        widgets/cButton.cpp
        widgets/cListBox.cpp
        widgets/cCheckBox.cpp
        widgets/cSlider.cpp
        widgets/cEditBox.cpp
        widgets/cTextItem.cpp
        widgets/cScrollBar.cpp
        widgets/cImageItem.cpp
        widgets/cScreenGetInput.cpp
        widgets/cMessageBox.cpp
        )

SET(XML_SRC xml/getattr.cpp xml/util.cpp)

# make imported targets for SDL
find_package(SDL 1.2)
add_library(SDL INTERFACE)
target_link_libraries(SDL INTERFACE ${SDL_LIBRARY})
target_include_directories(SDL INTERFACE ${SDL_INCLUDE_DIR})

find_package(SDL_image 1.2 REQUIRED)
add_library(SDL_image INTERFACE)
target_link_libraries(SDL_image INTERFACE ${SDL_IMAGE_LIBRARIES} )
target_include_directories(SDL INTERFACE ${SDL_IMAGE_INCLUDE_DIRS})

find_package(SDL_ttf 2.0 REQUIRED)
add_library(SDL_ttf INTERFACE)
target_link_libraries(SDL INTERFACE ${SDL_TTF_LIBRARIES})
target_include_directories(SDL_ttf INTERFACE ${SDL_TTF_INCLUDE_DIRS})

# there is no find_package(SDL_gfx), unfortunately.
add_library(WMEngine ${SRC} ${WIDGETS_SRC} ${INTERFACE_SRC} ${XML_SRC})
target_link_libraries(WMEngine PUBLIC SDL SDL_gfx SDL_ttf lua tinyxml2 SDL_image)
set_target_properties(WMEngine PROPERTIES CXX_STANDARD 14)

if (UNIX)
    target_compile_definitions(WMEngine PUBLIC LINUX=1)
endif(UNIX)

# Long-term goal: make the first one PRIVATE
target_include_directories(WMEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(WMEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
