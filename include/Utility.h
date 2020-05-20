//
// Created by Edge on 2020/5/11.
//

#ifndef JPEG_CODEC_UTILITY_H
#define JPEG_CODEC_UTILITY_H

#include <cstdint>
#include <cstddef>
#include <fstream>

template<typename T>
T operator >> (std::ifstream &ifs, T& a) {
    char buffer;
    a = 0;
    for(int i = 0;i < sizeof(T);++i) {
        ifs.read(&buffer, 1);
        a <<= 8;
        a |= static_cast<uint8_t>(buffer);
    }
}

bool checkData(std::ifstream &ifs, const char target[], int size) {
    char buffer;
    for (int i = 0; i < size-1; ++i) {
        ifs.read(&buffer, 1);
        if (target[i] != buffer) {
            return false;
        }
    }
    return true;
}

#endif //JPEG_CODEC_UTILITY_H
