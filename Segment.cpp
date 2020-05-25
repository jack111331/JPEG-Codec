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
    for (int i = 0; i < (int)data.m_componentSize; ++i) {
        ifs >> data.m_component[i];
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
    for (int i = 1; i <= 16; ++i) {
        os << "Length " << i << ": ";
        for (int j = 0; j < data.m_codeAmountOfBit[i]; ++j) {
            os << hexify(data.m_codeword[i][j]) << " ";
        }
        os << std::endl;
    }
    return os;
}

int HuffmanTable::getTableLength() const {
    return m_length;
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

std::ifstream &operator>>(std::ifstream &ifs, JPEG &data) {
    char header[3] = {};
    readData(ifs, header, 2);
    cout << "[INFO] Header " << hexify(header, 2) << "." << endl;
    if (!checkData(header, JPEG::MARKER_MAGIC_NUMBER, sizeof(JPEG::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] JPEG marker mismatch." << endl;
        exit(1);
    }
    readData(ifs, header, 2);
    bool isArrivedSOS = false;
    do {
        cout << "[INFO] Header " << hexify(header, 2) << "." << endl;
        if (SOS::checkSegment(header)) {
            ifs >> data.m_sos;
            std::cout << data.m_sos;
            isArrivedSOS = true;
        } else if (DRI::checkSegment(header)) {
            ifs >> data.m_dri;
            std::cout << data.m_dri;
        } else if (DHT::checkSegment(header)) {
            ifs >> data.m_dht[data.m_dhtSize++];
            std::cout << data.m_dht[data.m_dhtSize-1];
        } else if (SOF0::checkSegment(header)) {
            ifs >> data.m_sof0;
            std::cout << data.m_sof0;
        } else if (DQT::checkSegment(header)) {
            ifs >> data.m_dqt[data.m_dqtSize++];
            std::cout << data.m_dqt[data.m_dqtSize-1];

        } else if (APP0::checkSegment(header)) {
            ifs >> data.m_app0;
            std::cout << data.m_app0;
        } else {
            cout << "[ERROR] Unable to recognize header " << hexify(header, 2) << "." << endl;
            exit(1);
        }
        readData(ifs, header, 2);
    } while (!isArrivedSOS);

    uint8_t buffer;
    while (ifs >> buffer) {
        if (buffer == 0xff) {
            uint8_t indicator;
            ifs >> indicator;
            while (indicator == 0xff) {
                switch (indicator) {
                    case 0x00: {
                        data.m_scanData.push_back(buffer);
                        break;
                    }
                    case 0xd9:
                        goto EOI;
                    case 0xd0:
                    case 0xd1:
                    case 0xd2:
                    case 0xd3:
                    case 0xd4:
                    case 0xd5:
                    case 0xd6:
                    case 0xd7:
                        data.m_rstN = ((data.m_rstN + 1) & 0x7);
                        break;
                    default: {
                        data.m_scanData.push_back(indicator);
                    }

                }
                buffer = indicator;
                ifs >> indicator;
            }
        } else {
            data.m_scanData.push_back(buffer);
        }
    }
    EOI:;
}

std::ostream &operator<<(std::ostream &os, const JPEG &data) {
    os << data.m_app0;
    for (int i = 0; i < data.m_dqtSize; ++i) {
        os << data.m_dqt[i];
    }
    os << data.m_sof0;
    for (int i = 0; i < data.m_dhtSize; ++i) {
        os << data.m_dht[i];
    }
    os << data.m_dri;
    os << data.m_sos;
}
