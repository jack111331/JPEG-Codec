//
// Created by Edge on 2020/5/11.
//

#include <iostream>
#include <Utility.h>
#include "Segment.h"

using std::ifstream;
using std::cout;
using std::endl;

constexpr char APP0::MARKER_MAGIC_NUMBER[];
constexpr char APP0::IDENTIFIER_MAGIC_NUMBER[];
constexpr char JPEG::MARKER_MAGIC_NUMBER[];

std::ifstream &operator>>(std::ifstream &ifs, ColorType &data) {
    ifs >> data.r;
    ifs >> data.g;
    ifs >> data.b;
    return ifs;
}

std::ifstream &operator>>(std::ifstream &ifs, APP0 &data) {
    if (!checkData(ifs, APP0::MARKER_MAGIC_NUMBER, sizeof(APP0::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] APP0 marker mismatch." << endl;
        exit(1);
    }
    uint16_t length;
    ifs >> length;
    if (!checkData(ifs, APP0::IDENTIFIER_MAGIC_NUMBER, sizeof(APP0::IDENTIFIER_MAGIC_NUMBER))) {
        cout << "[ERROR] APP0 identifier mismatch." << endl;
        exit(1);
    }
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

    // TODO Add Length check

    return ifs;
}

APP0::~APP0() {
    if (m_thumbnailData) {
        delete[] m_thumbnailData;
    }
}

std::ifstream &operator>>(std::ifstream &ifs, DQT &data) {
    if (!checkData(ifs, DQT::MARKER_MAGIC_NUMBER, sizeof(DQT::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] DQT marker mismatch." << endl;
        exit(1);
    }
    uint16_t length;
    ifs >> length;

    while(length > 0) {
        // Precision and id
        ifs >> data.m_PTq[data.m_dqtSize];
        // Quantization table
        data.m_qs[data.m_dqtSize] = (data.m_PTq[data.m_dqtSize] & 0x10) ? (void *) (new uint16_t[64]) : (void *) (new uint8_t[64]);
        for (int i = 0; i < 64; ++i) {
            if((data.m_PTq[data.m_dqtSize] & 0x10)) {
                ifs >> ((uint16_t *)data.m_qs[data.m_dqtSize])[i];
            } else {
                ifs >> ((uint8_t *)data.m_qs[data.m_dqtSize])[i];
            }
        }
        ++data.m_dqtSize;
        length -= 1 + 64 * ((data.m_dqtSize&0x10) + 1);
    }

    return ifs;
}

DQT::~DQT() {
    for(int i = 0;i < m_dqtSize;++i) {
        if((m_PTq[m_dqtSize] & 0x10)) {
            delete[] (uint16_t *)m_qs[i];
        } else {
            delete[] (uint8_t *)m_qs[i];
        }
    }
}

std::ifstream & operator >> (std::ifstream &ifs, ColorComponent &data) {
    ifs >> data.m_id;
    ifs >> data.m_sampleFactor;
    ifs >> data.m_dqtId;
    return ifs;
}

std::ifstream & operator >> (std::ifstream &ifs, SOF0 &data) {
    if (!checkData(ifs, SOF0::MARKER_MAGIC_NUMBER, sizeof(SOF0::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] SOF0 marker mismatch." << endl;
        exit(1);
    }

    uint16_t length;
    ifs >> length;

    ifs >> data.m_precision;
    ifs >> data.m_height;
    ifs >> data.m_width;
    ifs >> data.m_componentSize;
    for(int i = 0;i < data.m_componentSize;++i) {
        ifs >> data.m_component[i];
    }
    return ifs;
}

std::ifstream &operator>>(std::ifstream &ifs, HuffmanTable &data) {
    ifs >> data.m_idAndType;
    // Quantization table
    data.m_length += 16;
    for(int i = 1;i <= 16;++i) {
        ifs >> data.m_codeAmountOfBit[i];
        data.m_length += data.m_codeAmountOfBit[i];
        if(data.m_codeAmountOfBit[i]) {
            data.m_codeword[i] = new uint8_t[data.m_codeAmountOfBit[i]];
        }
    }
    for(int i = 1;i <= 16;++i) {
        for(int j = 0;j < data.m_codeAmountOfBit[i];++i) {
            ifs >> data.m_codeword[i][j];
        }
    }
    return ifs;
}

int HuffmanTable::getTableLength() const {
    return m_length;
}


std::ifstream &operator>>(std::ifstream &ifs, DHT &data) {
    if (!checkData(ifs, DHT::MARKER_MAGIC_NUMBER, sizeof(DHT::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] DHT marker mismatch." << endl;
        exit(1);
    }

    uint16_t length;
    ifs >> length;

    while(length > 0) {
        ifs >> data.m_huffmanTable[data.m_dhtSize];
        length -= data.m_huffmanTable[data.m_dhtSize].getTableLength();
        ++data.m_dhtSize;
    }
    return ifs;
}

std::ifstream &operator>>(std::ifstream &ifs, JPEG &data) {
    if (!checkData(ifs, JPEG::MARKER_MAGIC_NUMBER, sizeof(JPEG::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] JPEG marker mismatch." << endl;
        exit(1);
    }
    ifs >> data.m_app0;
}