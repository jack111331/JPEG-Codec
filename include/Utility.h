//
// Created by Edge on 2020/5/11.
//

#ifndef JPEG_CODEC_UTILITY_H
#define JPEG_CODEC_UTILITY_H

#include <cstdint>
#include <cstddef>
#include <fstream>

std::ifstream &operator>>(std::ifstream &ifs, uint8_t &a) {
    ifs.read(reinterpret_cast<char *>(&a), sizeof(uint8_t));
    return ifs;
}

std::ifstream &operator>>(std::ifstream &ifs, uint16_t &a) {
    uint8_t buffer;
    a = 0;
    for(int i = 0;i < sizeof(uint16_t);++i) {
        ifs.read(reinterpret_cast<char *>(&buffer), 1);
        a <<= 8;
        a |= buffer;
    }
    return ifs;
}

static char hexTable[17] = "0123456789ABCDEF";

std::string hexify(uint8_t a) {
    std::string hex("0x");
    hex += (hexTable[(a >> 4)]);
    hex += hexTable[(a & 0x0f)];
    return hex;
}

std::string hexify(uint16_t a) {
    std::string hex("0x");
    hex += hexTable[(a >> 12)];
    hex += hexTable[((a >> 8) & 0x0f)];
    hex += hexTable[((a >> 4) & 0x0f)];
    hex += hexTable[(a & 0x0f)];
    return hex;
}

std::string hexify(const char a[], int size) {
    std::string hex("0x");
    for (int i = 0; i < size; ++i) {
        hex += hexTable[((a[i] >> 4) & 0x0f)];
        hex += hexTable[(a[i] & 0x0f)];
    }
    return hex;
}

void readData(std::ifstream &ifs, char buffer[], int size) {
    for (int i = 0; i < size; ++i) {
        ifs >> buffer[i];
    }
}

bool checkData(const char buffer[], const char target[], int size) {
    for (int i = 0; i < size - 1; ++i) {
        if (target[i] != buffer[i]) {
            return false;
        }
    }
    return true;
}

#endif //JPEG_CODEC_UTILITY_H
