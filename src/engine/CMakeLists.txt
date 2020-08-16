cmake_minimum_required(VERSION 3.10)

set(SRC CLog.cpp utils/DirPath.cpp utils/FileList.cpp sConfig.cpp utils/cKeyValueStore.cpp)

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
        interface/cInterfaceWindowXML.cpp
		interface/cTimer.cpp
		interface/constants.cpp
		interface/ffmpeg.cpp
		)

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

find_package(FFMPEG REQUIRED)

add_library(WMEngine STATIC ${SRC} ${WIDGETS_SRC} ${INTERFACE_SRC} ${XML_SRC})
target_link_libraries(WMEngine PUBLIC SDL2::SDL2 lua::lua Boost::headers tinyxml2 FFMPEG::FFMPEG)
target_link_libraries(WMEngine PRIVATE SDL2::TTF SDL2::Image)
set_target_properties(WMEngine PROPERTIES CXX_STANDARD 14)

if (UNIX)
    target_compile_definitions(WMEngine PUBLIC LINUX=1)
else()
	target_link_libraries(WMEngine PRIVATE shlwapi)
endif(UNIX)

# Long-term goal: make the first one PRIVATE
target_include_directories(WMEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(WMEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
