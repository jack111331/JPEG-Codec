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
public:
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

    uint8_t m_maxHorizontalComponent;
    uint8_t m_maxVerticalComponent;
};

struct BitStreamBuffer {
    int m_readLength = 0;
    uint8_t m_buffer = 0;

    friend std::ifstream &operator>>(std::ifstream &ifs, BitStreamBuffer &data);
};

class BitStream {
public:
    BitStream() : m_length(0), m_buffer(0) {};

    // Output if input is not enough to output length
    // If it's not enough, output remain length
    int putWord(BitStreamBuffer &input, int length);

    void clear();

    int getLength() const;

    uint16_t getWord() const;

    uint16_t m_buffer;
    int m_length;
};

class HuffmanTable {
public:
    HuffmanTable() : m_length(0), m_codeword{}, m_table{} {};

    ~HuffmanTable();

    friend std::ifstream &operator>>(std::ifstream &ifs, HuffmanTable &data);

    friend std::ostream &operator<<(std::ostream &os, const HuffmanTable &data);

    int getTableLength() const;

    bool getCode(uint16_t codeword, int length, uint8_t &output) const;

    uint8_t m_idAndType;
    uint8_t m_codeAmountOfBit[16 + 1];
    uint8_t *m_codeword[16 + 1];
    uint32_t m_table[16 + 1];
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

class DHTComponent {
public:
    friend std::ifstream &operator>>(std::ifstream &ifs, DHTComponent &data);

    friend std::ostream &operator<<(std::ostream &os, const DHTComponent &data);

    uint8_t m_id;
    uint8_t m_dcac;
    uint8_t m_dhtId;
};

class SOS {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xDA";

    static bool checkSegment(const char header[]);

    friend std::ifstream &operator>>(std::ifstream &ifs, SOS &data);

    friend std::ostream &operator<<(std::ostream &os, const SOS &data);

    uint8_t m_componentSize;
    DHTComponent m_component[4];
    uint8_t m_spectrumSelectionStart;
    uint8_t m_spectrumSelectionEnd;
    uint8_t m_spectrumSelection;
};

class JPEG;

class ComponentTable {
public:
    void read(std::ifstream &ifs, const ComponentTable *lastComponent, uint8_t verticalSize, uint8_t horizontalSize,
              const DHT &dcTable, const DHT &acTable,
              BitStreamBuffer &bsb);

    friend std::ostream &operator<<(std::ostream &os, const ComponentTable &data);

    ~ComponentTable();

    uint8_t m_verticalSize;
    uint8_t m_horizontalSize;
    uint8_t **m_table[8][8];

private:
    static float convertToCorrectCoefficient(uint16_t rawCoefficient, int length);

    float readDc(std::ifstream &ifs, const DHT &dcTable, BitStreamBuffer &bsb);

    struct ACValue {
        int state;
        int trailingZero;
        float value;
    };

    ACValue readAc(std::ifstream &ifs, const DHT &acTable, BitStreamBuffer &bsb);

    static constexpr int AC_ALL_ZERO = 0;
    static constexpr int AC_FOLLOWING_SIXTEEN_ZERO = 1;
    static constexpr int AC_NORMAL_STATE = 2;

};

class MCU {
public:
    void read(std::ifstream &ifs, const JPEG &jpeg, BitStreamBuffer &bsb);

    friend std::ostream &operator<<(std::ostream &os, const MCU &data);

    ComponentTable m_component[4];

    static constexpr int Y_COMPONENT = 0;
    static constexpr int CB_COMPONENT = 1;
    static constexpr int CR_COMPONENT = 2;

};

class MCUS {
public:
    MCUS() : m_lastMcu(nullptr) {};

    void read(std::ifstream &ifs, const JPEG &jpeg);

    friend std::ostream &operator<<(std::ostream &os, const MCUS &data);

    int m_mcuWidth, m_mcuHeight;
    MCU **m_mcu;
    MCU *m_lastMcu;

};

class JPEG {
public:
    constexpr static char MARKER_MAGIC_NUMBER[] = "\xFF\xD8";

    JPEG() : m_rstN(0), m_dqtSize(0), m_dhtSize{}, m_dht{} {};

    ~JPEG();

    friend std::ifstream &operator>>(std::ifstream &ifs, JPEG &data);

    friend std::ostream &operator<<(std::ostream &os, const JPEG &data);

    APP0 m_app0;
    int m_dqtSize;
    DQT m_dqt[4];
    SOF0 m_sof0;
    int m_dhtSize[2];
    DHT *m_dht[2][2];
    DRI m_dri;
    SOS m_sos;
    uint8_t m_rstN;
    MCUS m_mcus;

    static constexpr int DC_COMPONENT = 0;
    static constexpr int AC_COMPONENT = 1;
};

#endif //JPEG_CODEC_SEGMENT_H
