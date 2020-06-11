cmake_minimum_required(VERSION 3.6)
project(JPEG-Codec)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("include")

# Setup file to be compiled
set(JPEG_CODEC_SOURCE)
set(JPEG_CODEC_HEADER)

list(APPEND JPEG_CODEC_SOURCE
        main.cpp
        Segment.cpp
        Decoder.cpp
        )
list(APPEND JPEG_CODEC_HEADER
        include/Segment.h
        include/Decoder.h
        include/Utility.h
        )

set(all_code_files
        ${JPEG_CODEC_HEADER}
        ${JPEG_CODEC_SOURCE}
         )

add_executable(JPEG-Codec ${all_code_files})
target_link_libraries(JPEG-Codec)
file(RENAME ${CMAKE_BINARY_DIR}/JPEG-Codec.exe ${CMAKE_BINARY_DIR}/main.exe)