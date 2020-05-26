//
// Created by Edge on 2020/5/26.
//

#include "Decoder.h"
#include <iostream>

using namespace std;

void NaiveDequantization::process(JPEG &jpeg) {
    const SOF0 &sof0 = jpeg.m_sof0;
    int mcuWidth = jpeg.m_mcus.m_mcuWidth;
    int mcuHeight = jpeg.m_mcus.m_mcuHeight;
    for (int i = 0; i < mcuHeight; ++i) {
        for (int j = 0; j < mcuWidth; ++j) {
            for (int k = 0; k < sof0.m_componentSize; ++k) {
                const DQT *dqt = jpeg.m_dqt[sof0.m_component[k].m_dqtId];
                jpeg.m_mcus.m_mcu[i][j].m_component[k].multiplyWith(*dqt);
            }
        }
    }
}

void NaiveDezigzag::process(JPEG &jpeg) {
    int zigzagTable[8][8] = {
            {0,  1,  5,  6,  14, 15, 27, 28},
            {2,  4,  7,  13, 16, 26, 29, 42},
            {3,  8,  12, 17, 25, 30, 41, 43},
            {9,  11, 18, 24, 31, 40, 44, 53},
            {10, 19, 23, 32, 39, 45, 52, 54},
            {20, 22, 33, 38, 46, 51, 55, 60},
            {21, 34, 37, 47, 50, 56, 59, 61},
            {35, 36, 48, 49, 57, 58, 62, 63}
    };
    const SOF0 &sof0 = jpeg.m_sof0;
    int mcuWidth = jpeg.m_mcus.m_mcuWidth;
    int mcuHeight = jpeg.m_mcus.m_mcuHeight;
    for (int i = 0; i < mcuHeight; ++i) {
        for (int j = 0; j < mcuWidth; ++j) {
            for (int k = 0; k < sof0.m_componentSize; ++k) {
                ComponentTable componentTable;
                componentTable.init((sof0.m_component[i].m_sampleFactor & 0x0f),
                                    (sof0.m_component[i].m_sampleFactor >> 4));
                componentTable.replaceWith(jpeg.m_mcus.m_mcu[i][j].m_component[k], zigzagTable);
                jpeg.m_mcus.m_mcu[i][j].m_component[k] = componentTable;
            }
        }
    }
}

void IDCT::process(JPEG &jpeg) {
    
}

Decoder &Decoder::setDequantization(Dequantization *dequantizationStrategy) {
    m_dequantization = dequantizationStrategy;
}

Decoder &Decoder::setDezigzag(Dezigzag *dezigzagStrategy) {
    m_dezigzag = dezigzagStrategy;
}

Decoder &Decoder::setIDCT(IIDCT *idctStrategy) {
    m_idct = idctStrategy;
}

void Decoder::precess(JPEG &jpeg) {
    if (!m_dequantization) {
        cout << "[ERROR] Didn't assign dequantization strategy." << endl;
    }
    m_dequantization->process(jpeg);

    if (!m_dezigzag) {
        cout << "[ERROR] Didn't assign de ZIG-ZAG strategy." << endl;
    }
    m_dezigzag->process(jpeg);

    if (!m_idct) {
        cout << "[ERROR] Didn't assign IDCT strategy." << endl;
    }
    m_idct->process(jpeg);
}