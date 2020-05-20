//
// Created by Edge on 2020/5/11.
//

#ifndef JPEG_CODEC_SEGMENT_H
#define JPEG_CODEC_SEGMENT_H

#include <fstream>
#include <cstdint>

typedef struct ColorType {
    uint8_t r, g, b;

    friend std::ifstream &operator>>(std::ifstream &ifs, ColorType &data);
} Color;

class APP0 {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xE0";
    constexpr static char IDENTIFIER_MAGIC_NUMBER[] = "\x4A\x46\x49\x46\x00";

    APP0() : m_thumbnailData(nullptr) {};

    ~APP0();

    friend std::ifstream &operator>>(std::ifstream &ifs, APP0 &data);

    uint16_t m_version;
    uint8_t m_densityUnit;
    uint16_t m_xDensity;
    uint16_t m_yDensity;
    uint8_t m_xThumbnail;
    uint8_t m_yThumbnail;
    Color *m_thumbnailData;
};

class DQT {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xDB";

    DQT() : m_dqtSize(0), m_PTq{}, m_qs{} {};

    ~DQT();

    friend std::ifstream &operator>>(std::ifstream &ifs, DQT &data);

    int m_dqtSize;
    uint8_t m_PTq[4];
    void *m_qs[4];
};

class ColorComponent {
    friend std::ifstream &operator>>(std::ifstream &ifs, ColorComponent &data);

    uint8_t m_id;
    uint8_t m_sampleFactor;
    uint8_t m_dqtId;
};

class SOF0 {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xC0";

    friend std::ifstream &operator>>(std::ifstream &ifs, SOF0 &data);

    uint8_t m_precision;
    uint16_t m_height;
    uint16_t m_width;
    uint8_t m_componentSize;
    ColorComponent m_component[4];
};

class HuffmanTable {
public:
    HuffmanTable() : m_dhtSize(0), m_length(0), m_codeword{} {};
    ~HuffmanTable();

    friend std::ifstream &operator>>(std::ifstream &ifs, HuffmanTable &data);

    int getTableLength() const;

    int m_dhtSize;
    uint8_t m_idAndType;
    uint8_t m_codeAmountOfBit[16 + 1];
    uint8_t *m_codeword[16+1];
private:
    int m_length;
};

class DHT {
public:
    DHT() : m_dhtSize(0) {};
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xC4";

    friend std::ifstream &operator>>(std::ifstream &ifs, DHT &data);

    int m_dhtSize;
    HuffmanTable m_huffmanTable[4];
};

class JPEG {
public:
    APP0 m_app0;
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xD8";

    friend std::ifstream &operator>>(std::ifstream &ifs, JPEG &data);
};

#endif //JPEG_CODEC_SEGMENT_H
