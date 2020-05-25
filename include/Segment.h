//
// Created by Edge on 2020/5/11.
//

#ifndef JPEG_CODEC_SEGMENT_H
#define JPEG_CODEC_SEGMENT_H

#include <fstream>
#include <vector>
#include <cstdint>

typedef struct ColorType {
    uint8_t r, g, b;

    friend std::ifstream &operator>>(std::ifstream &ifs, ColorType &data);
} Color;

class APP0 {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xE0";
    constexpr static char IDENTIFIER_MAGIC_NUMBER[] = "\x4A\x46\x49\x46\x00";

    static bool checkSegment(const char header[]);

    APP0() : m_thumbnailData(nullptr) {};

    ~APP0();

    friend std::ifstream &operator>>(std::ifstream &ifs, APP0 &data);
    friend std::ostream &operator<<(std::ostream &os, const APP0 &data);


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

    static bool checkSegment(const char header[]);

    DQT() : m_PTq{}, m_qs{} {};

    ~DQT();

    friend std::ifstream &operator>>(std::ifstream &ifs, DQT &data);
    friend std::ostream &operator<<(std::ostream &os, const DQT &data);


    uint8_t m_PTq;
    void *m_qs;
};

class ColorComponent {
    friend std::ifstream &operator>>(std::ifstream &ifs, ColorComponent &data);
    friend std::ostream &operator<<(std::ostream &os, const ColorComponent &data);


    uint8_t m_id;
    uint8_t m_sampleFactor;
    uint8_t m_dqtId;
};

class SOF0 {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xC0";

    static bool checkSegment(const char header[]);

    friend std::ifstream &operator>>(std::ifstream &ifs, SOF0 &data);
    friend std::ostream &operator<<(std::ostream &os, const SOF0 &data);


    uint8_t m_precision;
    uint16_t m_height;
    uint16_t m_width;
    uint8_t m_componentSize;
    ColorComponent m_component[4];
};

class HuffmanTable {
public:
    HuffmanTable() : m_length(0), m_codeword{} {};
    ~HuffmanTable();

    friend std::ifstream &operator>>(std::ifstream &ifs, HuffmanTable &data);
    friend std::ostream &operator<<(std::ostream &os, const HuffmanTable &data);


    int getTableLength() const;

    uint8_t m_idAndType;
    uint8_t m_codeAmountOfBit[16 + 1];
    uint8_t *m_codeword[16+1];
private:
    int m_length;
};

class DHT {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xC4";

    static bool checkSegment(const char header[]);

    friend std::ifstream &operator>>(std::ifstream &ifs, DHT &data);
    friend std::ostream &operator<<(std::ostream &os, const DHT &data);


    HuffmanTable m_huffmanTable;
};

class DRI {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xDD";

    static bool checkSegment(const char header[]);

    friend std::ifstream &operator>>(std::ifstream &ifs, DRI &data);
    friend std::ostream &operator<<(std::ostream &os, const DRI &data);


    uint16_t m_restartInterval;
};

class SOS {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xDA";

    static bool checkSegment(const char header[]);

    friend std::ifstream &operator>>(std::ifstream &ifs, SOS &data);
    friend std::ostream &operator<<(std::ostream &os, const SOS &data);

    uint8_t m_componentSize;
    ColorComponent m_component[4];
    uint8_t m_spectrumSelectionStart;
    uint8_t m_spectrumSelectionEnd;
    uint8_t m_spectrumSelection;
};

class MCU {
public:
    friend std::ifstream &operator>>(std::ifstream &ifs, SOS &data);
    friend std::ostream &operator<<(std::ostream &os, const SOS &data);

    uint8_t m_componentSize;
    ColorComponent m_component[4];
    uint8_t m_spectrumSelectionStart;
    uint8_t m_spectrumSelectionEnd;
    uint8_t m_spectrumSelection;
};

class JPEG {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xD8";

    JPEG() : m_rstN(0), m_dqtSize(0), m_dhtSize(0) {};

    friend std::ifstream &operator>>(std::ifstream &ifs, JPEG &data);
    friend std::ostream &operator<<(std::ostream &os, const JPEG &data);

    APP0 m_app0;
    int m_dqtSize;
    DQT m_dqt[4];
    SOF0 m_sof0;
    int m_dhtSize;
    DHT m_dht[4];
    DRI m_dri;
    SOS m_sos;
    uint8_t m_rstN;
    std::vector<uint8_t> m_scanData;
};

#endif //JPEG_CODEC_SEGMENT_H
