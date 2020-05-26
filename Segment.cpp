//
// Created by Edge on 2020/5/11.
//

#include <iostream>
#include <Utility.h>
#include <cassert>
#include "Segment.h"

using std::ifstream;
using std::cout;
using std::endl;

constexpr char APP0::MARKER_MAGIC_NUMBER[];
constexpr char APP0::IDENTIFIER_MAGIC_NUMBER[];
constexpr char DQT::MARKER_MAGIC_NUMBER[];
constexpr char SOF0::MARKER_MAGIC_NUMBER[];
constexpr char DHT::MARKER_MAGIC_NUMBER[];
constexpr char DRI::MARKER_MAGIC_NUMBER[];
constexpr char SOS::MARKER_MAGIC_NUMBER[];
constexpr char JPEG::MARKER_MAGIC_NUMBER[];
constexpr int ComponentTable::AC_ALL_ZERO;
constexpr int ComponentTable::AC_FOLLOWING_SIXTEEN_ZERO;
constexpr int ComponentTable::AC_NORMAL_STATE;
constexpr int MCU::Y_COMPONENT;
constexpr int MCU::CB_COMPONENT;
constexpr int MCU::CR_COMPONENT;
constexpr int JPEG::AC_COMPONENT;
constexpr int JPEG::DC_COMPONENT;

std::ifstream &operator>>(std::ifstream &ifs, ColorType &data) {
    ifs >> data.r;
    ifs >> data.g;
    ifs >> data.b;
    return ifs;
}

bool APP0::checkSegment(const char header[]) {
    return checkData(header, APP0::MARKER_MAGIC_NUMBER, sizeof(APP0::MARKER_MAGIC_NUMBER));
}

std::ifstream &operator>>(std::ifstream &ifs, APP0 &data) {
    uint16_t length;
    ifs >> length;
    length -= 2;
    char identifier[6];
    readData(ifs, identifier, sizeof(APP0::IDENTIFIER_MAGIC_NUMBER) - 1);
    if (!checkData(identifier, APP0::IDENTIFIER_MAGIC_NUMBER, sizeof(APP0::IDENTIFIER_MAGIC_NUMBER))) {
        cout << "[ERROR] APP0 identifier mismatch." << endl;
        exit(1);
    }
    length -= 5;
    ifs >> data.m_version;
    ifs >> data.m_densityUnit;
    ifs >> data.m_xDensity;
    ifs >> data.m_yDensity;
    ifs >> data.m_xThumbnail;
    ifs >> data.m_yThumbnail;

    int thumbnailSize = data.m_xThumbnail * data.m_yThumbnail;
    if (thumbnailSize) {
        data.m_thumbnailData = new Color[thumbnailSize];
        for (int i = 0; i < thumbnailSize; ++i) {
            ifs >> data.m_thumbnailData[i];
        }
    }
    length -= 9 + thumbnailSize * sizeof(Color);
    assert(length == 0);

    return ifs;
}

std::ostream &operator<<(std::ostream &os, const APP0 &data) {
    os << "========= APP0 Start ========== " << std::endl;
    os << "Major Version: " << hexify((uint8_t) (data.m_version >> 8)) << std::endl;
    os << "Minor Version: " << hexify((uint8_t) (data.m_version & 0xff)) << std::endl;
    os << "Density Unit: " << hexify(data.m_densityUnit) << std::endl;
    os << "X Density: " << hexify(data.m_xDensity) << std::endl;
    os << "Y Density: " << hexify(data.m_yDensity) << std::endl;
    os << "X Thumbnail: " << hexify(data.m_xThumbnail) << std::endl;
    os << "Y Thumbnail: " << hexify(data.m_yThumbnail) << std::endl;
    os << "========= APP0 End ========== " << std::endl;
    return os;
}

APP0::~APP0() {
    if (m_thumbnailData) {
        delete[] m_thumbnailData;
    }
}

bool DQT::checkSegment(const char header[]) {
    return checkData(header, DQT::MARKER_MAGIC_NUMBER, sizeof(DQT::MARKER_MAGIC_NUMBER));
}

std::ifstream &operator>>(std::ifstream &ifs, DQT &data) {
    uint16_t length;
    ifs >> length;
    length -= 2;
    // Precision and id
    ifs >> data.m_PTq;
    // Quantization table
    data.m_qs = (data.m_PTq & 0x10) ? (void *) (new uint16_t[64])
                                    : (void *) (new uint8_t[64]);
    for (int i = 0; i < 64; ++i) {
        if ((data.m_PTq & 0x10)) {
            ifs >> ((uint16_t *) data.m_qs)[i];
        } else {
            ifs >> ((uint8_t *) data.m_qs)[i];
        }
    }
    length -= 1 + 64 * ((data.m_PTq & 0x10) + 1);
    assert(length == 0);

    return ifs;
}

std::ostream &operator<<(std::ostream &os, const DQT &data) {
    os << "========= DQT Start ========== " << std::endl;
    os << "Precision: " << (data.m_PTq >> 4) << std::endl;
    os << "ID: " << (data.m_PTq & 0x0f) << std::endl;
    os << "Quantization Table Content: " << std::endl;
    for (int j = 0; j < 64; ++j) {
        if ((data.m_PTq & 0x10)) {
            os << hexify(((uint16_t *) data.m_qs)[j]) << " ";
        } else {
            os << hexify(((uint8_t *) data.m_qs)[j]) << " ";
        }
        if (j % 8 == 7) {
            os << std::endl;
        }
    }
    os << "========= DQT End ========== " << std::endl;
    return os;
}

DQT::~DQT() {
    if ((m_PTq & 0x10)) {
        delete[] (uint16_t *) m_qs;
    } else {
        delete[] (uint8_t *) m_qs;
    }
}

std::ifstream &operator>>(std::ifstream &ifs, ColorComponent &data) {
    ifs >> data.m_id;
    ifs >> data.m_sampleFactor;
    ifs >> data.m_dqtId;
    return ifs;
}

std::ostream &operator<<(std::ostream &os, const ColorComponent &data) {
    os << "Color Component ID: " << hexify(data.m_id) << std::endl;
    os << "Sample Factor: " << hexify(data.m_sampleFactor) << std::endl;
    os << "Corresponding DQT ID: " << hexify(data.m_dqtId) << std::endl;
    return os;
}

bool SOF0::checkSegment(const char header[]) {
    return checkData(header, SOF0::MARKER_MAGIC_NUMBER, sizeof(SOF0::MARKER_MAGIC_NUMBER));
}

std::ifstream &operator>>(std::ifstream &ifs, SOF0 &data) {
    uint16_t length;
    ifs >> length;
    length -= 2;

    ifs >> data.m_precision;
    ifs >> data.m_height;
    ifs >> data.m_width;
    ifs >> data.m_componentSize;
    data.m_maxHorizontalComponent = data.m_maxVerticalComponent = 0;
    for (int i = 0; i < (int) data.m_componentSize; ++i) {
        ifs >> data.m_component[i];
        data.m_maxHorizontalComponent = std::max(data.m_maxHorizontalComponent,
                                                 (uint8_t) (data.m_component[i].m_sampleFactor >> 4));
        data.m_maxVerticalComponent = std::max(data.m_maxVerticalComponent,
                                               (uint8_t) (data.m_component[i].m_sampleFactor & 0x0f));
    }
    length -= 6 + data.m_componentSize * sizeof(ColorComponent);
    assert(length == 0);
    return ifs;
}

std::ostream &operator<<(std::ostream &os, const SOF0 &data) {
    os << "========= SOF0 Start ========== " << std::endl;
    os << "Precision: " << hexify(data.m_precision) << std::endl;
    os << "Height: " << hexify(data.m_height) << std::endl;
    os << "Width: " << hexify(data.m_width) << std::endl;
    for (int i = 0; i < data.m_componentSize; ++i) {
        os << "Color Component " << i << ": " << std::endl;
        os << data.m_component[i] << std::endl;
    }
    os << "========= SOF0 End ========== " << std::endl;
    return os;
}

std::ifstream &operator>>(std::ifstream &ifs, HuffmanTable &data) {
    ifs >> data.m_idAndType;
    // Quantization table
    data.m_length += 1;
    data.m_length += 16;
    for (int i = 1; i <= 16; ++i) {
        ifs >> data.m_codeAmountOfBit[i];
        data.m_length += data.m_codeAmountOfBit[i];
        if (data.m_codeAmountOfBit[i]) {
            data.m_table[i] = ((data.m_table[i - 1] + (uint32_t) data.m_codeAmountOfBit[i - 1]) << 1);
            data.m_codeword[i] = new uint8_t[data.m_codeAmountOfBit[i]];
        }
    }
    for (int i = 1; i <= 16; ++i) {
        for (int j = 0; j < data.m_codeAmountOfBit[i]; ++j) {
            ifs >> data.m_codeword[i][j];
        }
    }
    return ifs;
}

std::ostream &operator<<(std::ostream &os, const HuffmanTable &data) {
    os << "D/AC: " << ((data.m_idAndType >> 4) ? "AC" : "DC") << std::endl;
    os << "ID: " << (data.m_idAndType & 0x0f) << std::endl;
    uint16_t codeword = 0;
    for (int i = 1; i <= 16; ++i) {
        for (int j = 0; j < data.m_codeAmountOfBit[i]; ++j) {
            os << binify(codeword, i) << " " << hexify(data.m_codeword[i][j]) << std::endl;
            codeword++;
        }
        codeword <<= 1;
    }
    return os;
}

int HuffmanTable::getTableLength() const {
    return m_length;
}

bool HuffmanTable::getCode(uint16_t codeword, int length, uint8_t &output) const {
    if (m_codeAmountOfBit[length] && codeword >= m_table[length] && codeword - m_table[length] < m_codeAmountOfBit[length]) {
        output = m_codeword[length][codeword - m_table[length]];
        return true;
    }
    return false;
}

HuffmanTable::~HuffmanTable() {
    for (int i = 1; i <= 16; ++i) {
        if (m_codeAmountOfBit[i]) {
            delete[] m_codeword[i];
        }
    }
}

bool DHT::checkSegment(const char header[]) {
    return checkData(header, DHT::MARKER_MAGIC_NUMBER, sizeof(DHT::MARKER_MAGIC_NUMBER));
}

std::ifstream &operator>>(std::ifstream &ifs, DHT &data) {
    uint16_t length;
    ifs >> length;
    length -= 2;

    ifs >> data.m_huffmanTable;
    length -= data.m_huffmanTable.getTableLength();
    std::cout << data.m_huffmanTable.getTableLength() << std::endl;
    assert(length == 0);

    return ifs;
}

std::ostream &operator<<(std::ostream &os, const DHT &data) {
    os << "========= DHT Start ========== " << std::endl;
    os << data.m_huffmanTable;
    os << "========= DHT End ========== " << std::endl;
    return os;
}

bool DRI::checkSegment(const char header[]) {
    return checkData(header, DRI::MARKER_MAGIC_NUMBER, sizeof(DRI::MARKER_MAGIC_NUMBER));
}

std::ifstream &operator>>(std::ifstream &ifs, DRI &data) {
    uint16_t length;
    ifs >> length;

    ifs >> data.m_restartInterval;
    return ifs;
}

std::ostream &operator<<(std::ostream &os, const DRI &data) {
    os << "========= DRI Start ========== " << std::endl;
    os << "Every " << hexify(data.m_restartInterval) << " number of MCU will be 1 RSTn tag" << std::endl;
    os << "========= DRI End ========== " << std::endl;
    return os;
}

std::ifstream &operator>>(std::ifstream &ifs, DHTComponent &data) {
    ifs >> data.m_id;
    ifs >> data.m_dcac;
    ifs >> data.m_dhtId;
    return ifs;
}

std::ostream &operator<<(std::ostream &os, const DHTComponent &data) {
    os << "Component ID: " << hexify(data.m_id) << std::endl;
    os << "DC/AC id: " << hexify(data.m_dcac) << std::endl;
    os << "Corresponding DHT ID: " << hexify(data.m_dhtId) << std::endl;
    return os;
}

bool SOS::checkSegment(const char header[]) {
    return checkData(header, SOS::MARKER_MAGIC_NUMBER, sizeof(SOS::MARKER_MAGIC_NUMBER));
}

std::ifstream &operator>>(std::ifstream &ifs, SOS &data) {
    uint16_t length;
    ifs >> length;

    ifs >> data.m_componentSize;
    for (int i = 0; i < data.m_componentSize; ++i) {
        ifs >> data.m_component[i];
    }
    ifs >> data.m_spectrumSelectionStart;
    ifs >> data.m_spectrumSelectionEnd;
    ifs >> data.m_spectrumSelection;
    return ifs;
}

std::ostream &operator<<(std::ostream &os, const SOS &data) {
    os << "========= SOS Start ========== " << std::endl;
    os << "Component Size: " << hexify(data.m_componentSize) << std::endl;
    for (int i = 0; i < data.m_componentSize; ++i) {
        os << "Component " << i << " of " << hexify(data.m_componentSize) << std::endl;
        os << data.m_component[i];
    }
    os << "Spectrum Selection Start: " << hexify(data.m_spectrumSelectionStart) << std::endl;
    os << "Spectrum Selection End: " << hexify(data.m_spectrumSelectionEnd) << std::endl;
    os << "Spectrum Selection: " << hexify(data.m_spectrumSelection) << std::endl;
    os << "========= SOS End ========== " << std::endl;
    return os;
}

std::ifstream &operator>>(std::ifstream &ifs, BitStreamBuffer &data) {
    ifs >> data.m_buffer;
    data.m_readLength = 0;
    return ifs;
}

int BitStream::putWord(BitStreamBuffer &input, int length) {
    if (input.m_readLength == 8) {
        return length;
    } else if (length + m_length % 8 > 8) {
        int bitToRead = (8 - m_length % 8);
        m_buffer <<= bitToRead;
        m_buffer |= ((input.m_buffer >> (8 - bitToRead - input.m_readLength)) & ((1 << bitToRead) - 1));
        input.m_readLength += (8 - m_length % 8);
        m_length += (8 - m_length % 8);
        return (length + m_length) % 8;
    } else {
        m_buffer <<= length;
        m_buffer |= ((input.m_buffer >> (8 - length - input.m_readLength)) & ((1 << length) - 1));
        input.m_readLength += length;
        m_length += length;
        return 0;
    }
}

void BitStream::clear() {
    m_buffer = 0;
    m_length = 0;
}

int BitStream::getLength() const {
    return m_length;
}

uint16_t BitStream::getWord() const {
    return m_buffer;
}

void ComponentTable::read(std::ifstream &ifs, const ComponentTable *lastComponent, uint8_t verticalSize,
                          uint8_t horizontalSize, const DHT &dcTable,
                          const DHT &acTable, BitStreamBuffer &bsb) {
    m_verticalSize = verticalSize;
    m_horizontalSize = horizontalSize;
    for (int i = 0; i < verticalSize; ++i) {
        for (int j = 0; j < horizontalSize; ++j) {
            m_table[i][j] = new uint8_t *[verticalSize];
            for (int k = 0; k < verticalSize; ++k) {
                m_table[i][j][k] = new uint8_t[horizontalSize];
            }
            uint32_t count = 1;
            m_table[0][0][i][j] = readDc(ifs, dcTable, bsb);
            if (lastComponent) {
                m_table[0][0][i][j] += lastComponent->m_table[0][0][i][j];
            }
            while (count < 64) {
                ComponentTable::ACValue acValue = readAc(ifs, acTable, bsb);
                switch (acValue.state) {
                    case AC_ALL_ZERO : {
                        while (count < 64) {
                            m_table[count >> 3][count & 0x07][i][j] = 0.0;
                            ++count;
                        }
                        break;
                    }
                    case AC_FOLLOWING_SIXTEEN_ZERO : {
                        for (int k = 0; k < 16; ++k) {
                            m_table[count >> 3][count & 0x07][i][j] = 0.0;
                            ++count;
                        }
                        break;
                    }
                    case AC_NORMAL_STATE : {
                        for (int k = 0; k < acValue.trailingZero; ++k) {
                            m_table[count >> 3][count & 0x07][i][j] = 0.0;
                            ++count;
                        }
                        m_table[count >> 3][count & 0x07][i][j] = acValue.value;
                        ++count;
                        break;
                    }
                }
            }
        }
    }
}

std::ostream &operator<<(std::ostream &os, const ComponentTable &data) {
    for (int i = 0; i < data.m_verticalSize; ++i) {
        for (int j = 0; j < data.m_horizontalSize; ++j) {
            os << "=========== Sample of (" << i << ", " << j << ") Start =========" << std::endl;
            for (int k = 0; k < 8; ++k) {
                for (int l = 0; l < 8; ++l) {
                    os << data.m_table[k][l][i][j] << " ";
                }
                os << std::endl;
            }
            os << "=========== Sample of (" << i << ", " << j << ") End =========" << std::endl;
        }
    }
    return os;
}


float ComponentTable::convertToCorrectCoefficient(uint16_t rawCoefficient, int length) {
    if ((rawCoefficient >> (length - 1)) == 1) {
        rawCoefficient = ~rawCoefficient;
    }
    return rawCoefficient;
}

float ComponentTable::readDc(std::ifstream &ifs, const DHT &dcTable, BitStreamBuffer &bsb) {
    ifs >> bsb;
    BitStream bs;
    bs.putWord(bsb, 1);
    uint8_t output;
    // Decode n from huffman table
    while (!dcTable.m_huffmanTable.getCode(bs.getWord(), bs.getLength(), output)) {
        if (bs.putWord(bsb, 1)) {
            ifs >> bsb;
        }
    }
    bs.clear();
    while ((output = bs.putWord(bsb, output))) {
        ifs >> bsb;
    }
    uint16_t rawCoefficient = bs.getWord();
    bs.clear();
    return convertToCorrectCoefficient(rawCoefficient, output);
}

ComponentTable::ACValue ComponentTable::readAc(std::ifstream &ifs, const DHT &acTable, BitStreamBuffer &bsb) {
    ifs >> bsb;
    BitStream bs;
    bs.putWord(bsb, 1);
    uint8_t output;
    // Decode n from huffman table
    while (!acTable.m_huffmanTable.getCode(bs.getWord(), bs.getLength(), output)) {
        if (bs.putWord(bsb, 1)) {
            ifs >> bsb;
        }
    }
    ComponentTable::ACValue acValue;
    if (output == 0x00) {
        acValue.state = AC_ALL_ZERO;
    } else if (output == 0xF0) {
        acValue.state = AC_FOLLOWING_SIXTEEN_ZERO;
    } else {
        acValue.state = AC_NORMAL_STATE;
        acValue.trailingZero = (output >> 4);
        bs.clear();
        while ((output = bs.putWord(bsb, (output & 0x0F)))) {
            ifs >> bsb;
        }
        uint16_t rawCoefficient = bs.getWord();
        bs.clear();
        acValue.value = convertToCorrectCoefficient(rawCoefficient, output);
    }
    return acValue;
}

ComponentTable::~ComponentTable() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            for (int k = 0; k < m_verticalSize; ++k) {
                delete[] m_table[i][j][k];
            }
            delete[] m_table[i][j];
        }
    }
}

void MCU::read(std::ifstream &ifs, const JPEG &jpeg, BitStreamBuffer &bsb) {
    for (int i = 0; i < jpeg.m_sof0.m_componentSize; ++i) {
        // HuffmanTable *ac, *dc
        DHT *dc = jpeg.m_dht[JPEG::DC_COMPONENT][jpeg.m_sos.m_component[i].m_dcac >> 4];
        DHT *ac = jpeg.m_dht[JPEG::AC_COMPONENT][jpeg.m_sos.m_component[i].m_dcac & 0x0f];
        if (jpeg.m_mcus.m_lastMcu) {
            m_component[i].read(ifs, &jpeg.m_mcus.m_lastMcu->m_component[i],
                                (jpeg.m_sof0.m_component[i].m_sampleFactor >> 4),
                                (jpeg.m_sof0.m_component[i].m_sampleFactor & 0x0f), *dc, *ac, bsb);

        } else {
            m_component[i].read(ifs, nullptr,
                                (jpeg.m_sof0.m_component[i].m_sampleFactor >> 4),
                                (jpeg.m_sof0.m_component[i].m_sampleFactor & 0x0f), *dc, *ac, bsb);
        }
    }
}

std::ostream &operator<<(std::ostream &os, const MCU &data) {
    for (int i = 0; i < 3; ++i) {
        os << "=========== Component " << i << " Start =========" << std::endl;
        os << data.m_component[i];
        os << "=========== Component " << i << " End =========" << std::endl;
    }
    return os;
}

void MCUS::read(std::ifstream &ifs, const JPEG &jpeg) {
    // Calculate how many mcu in row and column
    m_mcuWidth = (jpeg.m_sof0.m_width - 1) / (jpeg.m_sof0.m_maxHorizontalComponent) + 1;
    m_mcuHeight = (jpeg.m_sof0.m_height - 1) / (jpeg.m_sof0.m_maxVerticalComponent) + 1;
    m_mcu = new MCU *[m_mcuHeight];
    BitStreamBuffer bsb;
    for (int i = 0; i < m_mcuHeight; ++i) {
        m_mcu[i] = new MCU[m_mcuWidth];
        for (int j = 0; j < m_mcuWidth; ++j) {
            m_mcu[i][j].read(ifs, jpeg, bsb);
            m_lastMcu = &m_mcu[i][j];
        }
    }
}

std::ostream &operator<<(std::ostream &os, const MCUS &data) {
    for (int i = 0; i < data.m_mcuHeight; ++i) {
        for (int j = 0; j < data.m_mcuWidth; ++j) {
            os << "========= MCU (" << i << ", " << j << ") Start ========== " << std::endl;
            os << data.m_mcu[i][j];
            os << "========= MCU (" << i << ", " << j << ") End ========== " << std::endl;
        }
    }
    return os;
}

std::ifstream &operator>>(std::ifstream &ifs, JPEG &data) {
    char header[3] = {};
    readData(ifs, header, 2);
    cout << "[INFO] Header " << hexify(header, 2) << "." << endl;
    if (!checkData(header, JPEG::MARKER_MAGIC_NUMBER, sizeof(JPEG::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] JPEG marker mismatch." << endl;
        exit(1);
    }
    readData(ifs, header, 2);
    do {
        cout << "[INFO] Header " << hexify(header, 2) << "." << endl;
        if (SOS::checkSegment(header)) {
            ifs >> data.m_sos;
            std::cout << data.m_sos;
            break;
        } else if (DRI::checkSegment(header)) {
            ifs >> data.m_dri;
            std::cout << data.m_dri;
        } else if (DHT::checkSegment(header)) {
            DHT *newDHT = new DHT();
            ifs >> *newDHT;
            data.m_dht[newDHT->m_huffmanTable.m_idAndType & 4][data.m_dhtSize[newDHT->m_huffmanTable.m_idAndType &
                                                                              4]++] = newDHT;
            std::cout << *data.m_dht[newDHT->m_huffmanTable.m_idAndType & 4][
                    data.m_dhtSize[newDHT->m_huffmanTable.m_idAndType & 4] - 1];
        } else if (SOF0::checkSegment(header)) {
            ifs >> data.m_sof0;
            std::cout << data.m_sof0;
        } else if (DQT::checkSegment(header)) {
            ifs >> data.m_dqt[data.m_dqtSize++];
            std::cout << data.m_dqt[data.m_dqtSize - 1];

        } else if (APP0::checkSegment(header)) {
            ifs >> data.m_app0;
            std::cout << data.m_app0;
        } else {
            cout << "[ERROR] Unable to recognize header " << hexify(header, 2) << "." << endl;
            exit(1);
        }
        readData(ifs, header, 2);
    } while (true);

    data.m_mcus.read(ifs, data);
    // TODO Read EOI
}

std::ostream &operator<<(std::ostream &os, const JPEG &data) {
    os << data.m_app0;
    for (int i = 0; i < data.m_dqtSize; ++i) {
        os << data.m_dqt[i];
    }
    os << data.m_sof0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < data.m_dhtSize[i]; ++j) {
            os << data.m_dht[i][j];
        }
    }
    os << data.m_dri;
    os << data.m_sos;
}

JPEG::~JPEG() {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < m_dhtSize[i]; ++j) {
            delete m_dht[i][j];
        }
    }
}