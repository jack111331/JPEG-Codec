//
// Created by Edge on 2020/5/11.
//

#ifndef JPEG_CODEC_UTILITY_H
#define JPEG_CODEC_UTILITY_H

#include <cstdint>
#include <cstddef>
#include <fstream>

bool checkData(std::ifstream &ifs, uint64_t target, int size) {
    char buffer;
    for (int i = size-1; i >= 0; --i) {
        ifs.read(&buffer, 1);
        if (((target >> (i << 3)) & 0xFF) != static_cast<uint8_t>(buffer)) {
            return false;
        }
    }
    return true;
}

#endif //JPEG_CODEC_UTILITY_H
