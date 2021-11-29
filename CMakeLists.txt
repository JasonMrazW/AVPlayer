cmake_minimum_required(VERSION 3.20)
project(AVPlayer)

# ------- Set Vars ------- #



set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMAKE")
set(SOURCE_FILES
        src/main.cpp src/SDLActivity.hpp
        src/SDLActivity.cpp
        src/parser/header/YUVFileData.h
        src/parser/header/IImageParser.h src/parser/YUVImageParser.cpp src/parser/header/YUVImageParser.h src/parser/header/RGBImageParser.h src/parser/RGBImageParser.cpp)

Message("")
Message( STATUS "SOURCE entry point : " ${SOURCE_FILES} )
Message("")

# ------- End Set Vars --- #


# ------- Executable ----- #
add_executable(AVPlayer ${SOURCE_FILES})

# ------- Finds ---------- #
set(SDL2_DIR "library/sdl2/sdl2-config.cmake")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(AVPlayer ${SDL2_LIBRARIES})

message("${SDL2_INCLUDE_DIRS}")
message("${SDL2_LIBRARIES}")


file(COPY resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# ------- End Finds ------ #
# ------- Inc & Link ---- #
IF(APPLE)
    MESSAGE("2")
    SET(CMAKE_OSX_ARCHITECTURES "arm64;" CACHE STRING "Build architectures for Mac OS X" FORCE)
ENDIF(APPLE)
# ------- End ----------- #