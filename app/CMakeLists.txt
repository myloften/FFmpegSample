# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

set(distribution_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library( ffmpeg-lib
             SHARED
             src/main/cpp/ffmpeg-lib.cpp
             src/main/cpp/decode-video.cpp
             src/main/cpp/decode-audio.cpp
             src/main/cpp/live555-lib.cpp
             src/main/cpp/testRTSPClient.cpp)

add_library( libffmpeg
             SHARED
             IMPORTED)
set_target_properties( libffmpeg
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/libffmpeg.so)

include_directories(${distribution_DIR}/include)

add_library( liblive555
             SHARED
             IMPORTED)
set_target_properties( liblive555
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/${ANDROID_ABI}/liblive555.so)

include_directories(${distribution_DIR}/live555/BasicUsageEnvironment/include)
include_directories(${distribution_DIR}/live555/groupsock/include)
include_directories(${distribution_DIR}/live555/liveMedia/include)
include_directories(${distribution_DIR}/live555/UsageEnvironment/include)

find_library(log-lib
              log )
include_directories(${distribution_DIR}/utils)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

target_link_libraries( ffmpeg-lib
                       libffmpeg
                       liblive555
                       android
                       ${log-lib} )
