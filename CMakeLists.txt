cmake_minimum_required(VERSION 3.20)
project(AVPlayer)

# ------- Set Vars ------- #


# require C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMAKE")
message("=======CMAKE_cur_ver: ${CMAKE_VERSION}")

set(SOURCE_FILES
        src/main.cpp
        src/player/SDLImagePlayer.hpp
        src/player/SDLImagePlayer.cpp
        src/parser/header/YUVFileData.h
        src/parser/header/IImageParser.h
        src/parser/YUVImageParser.cpp
        src/parser/header/YUVImageParser.h
        src/parser/header/RGBImageParser.h
        src/parser/RGBImageParser.cpp
        src/player/SDLAudioPlayer.cpp
        src/player/SDLAudioPlayer.h
        src/io/AVFileUtil.cpp
        src/io/AVFileUtil.h
        src/parser/H264Parser.cpp
        src/parser/header/H264Parser.h src/parser/AACParser.cpp src/parser/header/AACParser.h src/util/BinaryUtil.cpp src/util/BinaryUtil.h src/parser/FLVParaser.cpp src/parser/header/FLVParaser.h src/parser/header/FLVAMFInfo.h src/parser/header/FLVVideoTag.h src/parser/header/FLVAudioTag.h src/basic/basic.cpp src/basic/basic.h src/io/socket_server.cpp src/io/socket_server.h src/io/socket_client.cpp src/io/socket_client.h src/io/socket_tcp_server.cpp src/io/socket_tcp_server.h src/io/socket_udp_server.cpp src/io/socket_udp_server.h src/io/socket_tcp_client.cpp src/io/socket_tcp_client.h src/io/socket_info.h src/io/socket_udp_client.cpp src/io/socket_udp_client.h src/parser/RTPParser.cpp src/parser/header/RTPParser.h src/ffmpeg/ff_main_sample.cpp src/ffmpeg/ff_main_sample.h src/player/SDLAudioStreamPlayer.cpp src/player/SDLAudioStreamPlayer.h src/util/ConvertUtil.cpp src/util/ConvertUtil.h src/player/avplayer/AV_FSDPlayer.cpp src/player/avplayer/header/AV_FSDPlayer.h src/player/avplayer/AV_Render_SDL.cpp src/player/avplayer/header/AV_Render_SDL.h src/player/avplayer/AV_Render_Video.cpp src/player/avplayer/header/AV_Render_Video.h src/player/avplayer/AV_Render_Audio.cpp src/player/avplayer/header/AV_Render_Audio.h src/player/avplayer/header/AV_Render_Interface.h src/player/avplayer/AV_Demuxer.cpp src/player/avplayer/header/AV_Demuxer.h src/thread/ThreadSafeQueue.h src/player/avplayer/header/AV_Decoder.h src/player/avplayer/AV_Decoder_Video.cpp src/player/avplayer/header/AV_Decoder_Video.h src/player/avplayer/AV_Decoder_Audio.cpp src/player/avplayer/header/AV_Decoder_Audio.h src/player/avplayer/header/AV_PCM_DataItem.h)

Message("   ")
Message(STATUS "SOURCE entry point : " ${SOURCE_FILES})
Message("  2323")

# ------- End Set Vars --- #

#-------FFMpeg--------#
if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
    # FFmpeg的安装目录，可以通过命令"brew info ffmpeg"获取
    set(FFMPEG_DIR /opt/homebrew/Cellar/ffmpeg/4.4_2)
    # 头文件搜索路径
    include_directories(${FFMPEG_DIR}/include/)
    # 动态链接库或静态链接库的搜索路径
    link_directories(${FFMPEG_DIR}/lib/)
endif ()

if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    # FFmpeg的安装目录，可以通过命令"brew info ffmpeg"获取
    set(FFMPEG_DIR /usr/local/Cellar/ffmpeg/4.4.1_3)
    # 头文件搜索路径
    include_directories(${FFMPEG_DIR}/include/)
    # 动态链接库或静态链接库的搜索路径
    link_directories(${FFMPEG_DIR}/lib/)
endif ()
# ------- Executable ----- #

add_executable(AVPlayer ${SOURCE_FILES})

# ------- Finds ---------- #
set(SDL2_DIR "library/sdl2/sdl2-config.cmake")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})


target_link_libraries(
        AVPlayer
        ${SDL2_LIBRARIES}
        avcodec
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
)

message("${SDL2_INCLUDE_DIRS}")
message("${SDL2_LIBRARIES}")


file(COPY resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# ------- End Finds ------ #
# ------- Inc & Link ---- #
message("-- CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

IF (APPLE)
    if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
        SET(CMAKE_OSX_ARCHITECTURES "arm64;" CACHE STRING "Build architectures for Mac OS X" FORCE)
    endif ()
ENDIF (APPLE)
# ------- End ----------- #