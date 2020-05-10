//
// Created by Edge on 2020/5/11.
//

#ifndef JPEG_CODEC_SEGMENT_H
#define JPEG_CODEC_SEGMENT_H

#include <fstream>

class APP0 {
public:
    constexpr static uint16_t MARKER_MAGIC_NUMBER = 0xFFE0;

    friend std::ifstream & operator >> (std::ifstream &ifs, APP0 &data);
};

class JPEG {
public:
    APP0 m_app0;

    constexpr static uint16_t MARKER_MAGIC_NUMBER = 0xFFD8;

    friend std::ifstream & operator >> (std::ifstream &ifs, JPEG &data);
};

#endif //JPEG_CODEC_SEGMENT_H
