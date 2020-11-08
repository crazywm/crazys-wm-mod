if(UNIX)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET GLOBAL
            #        libavdevice
            #        libavfilter
            libavformat
            libavcodec
            #        libswresample
            libswscale
            libavutil
            )
    add_library(FFMPEG::FFMPEG ALIAS PkgConfig::LIBAV)
else()
    set(prefix ${PROJECT_SOURCE_DIR}/deps/ffmpeg)
	
    find_library(AVFORMAT_LIB avformat PATHS ${prefix}/lib REQUIRED)
    find_library(AVCODEC_LIB avcodec PATHS ${prefix}/lib REQUIRED)
    find_library(SWSCALE_LIB swscale PATHS ${prefix}/lib REQUIRED)
    find_library(AVUTIL_LIB avutil PATHS ${prefix}/lib REQUIRED)
    set(FFMPEG_LIBRARIES ${AVFORMAT_LIB} ${AVCODEC_LIB} ${SWSCALE_LIB} ${AVUTIL_LIB})

	add_library(FFMPEG INTERFACE)
    target_link_libraries(FFMPEG INTERFACE ${FFMPEG_LIBRARIES})
	
    target_include_directories(FFMPEG INTERFACE ${prefix}/include)
    install(DIRECTORY ${prefix}/bin/ DESTINATION ${CMAKE_INSTALL_BINDIR})
	
    add_library(FFMPEG::FFMPEG ALIAS FFMPEG)
endif()
