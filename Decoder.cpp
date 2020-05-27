//
// Created by Edge on 2020/5/26.
//

#include "Decoder.h"
#include <iostream>
#include <cmath>

using namespace std;

constexpr int Image::R_COMPONENT;
constexpr int Image::G_COMPONENT;
constexpr int Image::B_COMPONENT;

void NaiveDequantization::process(JPEG &jpeg) {
    const SOF0 &sof0 = jpeg.m_sof0;
    int mcuWidth = jpeg.m_mcus.m_mcuWidth;
    int mcuHeight = jpeg.m_mcus.m_mcuHeight;
    for (int i = 0; i < mcuHeight; ++i) {
        for (int j = 0; j < mcuWidth; ++j) {
            for (int k = 0; k < sof0.m_componentSize; ++k) {
                const DQT *dqt = jpeg.m_dqt[sof0.m_component[k].m_dqtId];
                jpeg.m_mcus.m_mcu[i][j].m_component[k]->multiplyWith(*dqt);
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
                ComponentTable *componentTable = new ComponentTable();
                componentTable->init((sof0.m_component[k].m_sampleFactor & 0x0f),
                                    (sof0.m_component[k].m_sampleFactor >> 4));
                componentTable->replaceWith(*jpeg.m_mcus.m_mcu[i][j].m_component[k], zigzagTable);
                delete jpeg.m_mcus.m_mcu[i][j].m_component[k];
                jpeg.m_mcus.m_mcu[i][j].m_component[k] = componentTable;
            }
        }
    }
}

void EnhancedDezigzag::process(JPEG &jpeg) {
    int swapTable[8][8] = {
            {0,  1,  5,  6,  14, 15, 27, 28},
            {15, 14, 28, 13, 16, 26, 29, 42},
            {27, 42, 27, 42, 25, 30, 41, 43},
            {29, 26, 27, 29, 31, 40, 44, 53},
            {53, 42, 43, 53, 39, 45, 52, 54},
            {40, 41, 42, 52, 46, 51, 55, 60},
            {55, 52, 51, 60, 60, 56, 59, 61},
            {56, 59, 61, 60, 60, 61, 62, 63}
    };
    const SOF0 &sof0 = jpeg.m_sof0;
    int mcuWidth = jpeg.m_mcus.m_mcuWidth;
    int mcuHeight = jpeg.m_mcus.m_mcuHeight;
    for (int i = 0; i < mcuHeight; ++i) {
        for (int j = 0; j < mcuWidth; ++j) {
            for (int k = 0; k < sof0.m_componentSize; ++k) {
                jpeg.m_mcus.m_mcu[i][j].m_component[k]->inPlaceReplaceWith(swapTable);
            }
        }
    }
}

float IIDCT::coefficientPrecompute(int x, int y) {
    if (x == 0 && y == 0) {
        return 0.5f;
    } else if (x == 0 || y == 0) {
        return 1 / sqrt(2);
    } else {
        return 1.0f;
    }
}

void NaiveIDCT::process(JPEG &jpeg) {
    const SOF0 &sof0 = jpeg.m_sof0;
    int mcuWidth = jpeg.m_mcus.m_mcuWidth;
    int mcuHeight = jpeg.m_mcus.m_mcuHeight;
    for (int i = 0; i < mcuHeight; ++i) {
        for (int j = 0; j < mcuWidth; ++j) {
            for (int k = 0; k < sof0.m_componentSize; ++k) {
                ComponentTable *componentTable = new ComponentTable();
                componentTable->init((sof0.m_component[k].m_sampleFactor & 0x0f),
                                    (sof0.m_component[k].m_sampleFactor >> 4));
                performIdctOnComponentTable(*jpeg.m_mcus.m_mcu[i][j].m_component[k], *componentTable);
                delete jpeg.m_mcus.m_mcu[i][j].m_component[k];
                jpeg.m_mcus.m_mcu[i][j].m_component[k] = componentTable;
            }
        }
    }
}

void NaiveIDCT::performIdctOnComponentTable(ComponentTable &table, ComponentTable &result) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            for (int k = 0; k < table.m_verticalSize; ++k) {
                for (int l = 0; l < table.m_horizontalSize; ++l) {
                    result.m_table[i][j][k][l] = computeCoefficientAtIndex(table, k, l, i, j);
                }
            }
        }
    }
}

float
NaiveIDCT::computeCoefficientAtIndex(ComponentTable &table, int verticalComponent, int horizonComponent, int i, int j) {
    float result = 0;
    const double pi = acos(-1);
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            // TODO computeCoefficientAtIndex can be precompute to table lookup
            result += coefficientPrecompute(x, y) * cos(0.0625f * (i * 2 + 1) * x * pi) *
                      cos(0.0625f * (j * 2 + 1) * y * pi) * table.m_table[x][y][verticalComponent][horizonComponent];
        }
    }
    return result * 0.25f;
}

void DimensionReductionIDCT::process(JPEG &jpeg) {
    const SOF0 &sof0 = jpeg.m_sof0;
    int mcuWidth = jpeg.m_mcus.m_mcuWidth;
    int mcuHeight = jpeg.m_mcus.m_mcuHeight;
    for (int i = 0; i < mcuHeight; ++i) {
        for (int j = 0; j < mcuWidth; ++j) {
            for (int k = 0; k < sof0.m_componentSize; ++k) {
                ComponentTable *componentTable = new ComponentTable();
                componentTable->init((sof0.m_component[k].m_sampleFactor & 0x0f),
                                    (sof0.m_component[k].m_sampleFactor >> 4));
                performIdctOnComponentTable(*jpeg.m_mcus.m_mcu[i][j].m_component[k], *componentTable);
                delete jpeg.m_mcus.m_mcu[i][j].m_component[k];
                jpeg.m_mcus.m_mcu[i][j].m_component[k] = componentTable;
            }
        }
    }
}

void DimensionReductionIDCT::performIdctOnComponentTable(ComponentTable &table, ComponentTable &result) {
    const double pi = acos(-1);
    float precompute[8][8];
    for (int k = 0; k < table.m_verticalSize; ++k) {
        for (int l = 0; l < table.m_horizontalSize; ++l) {
            for (int j = 0; j < 8; ++j) {
                for (int x = 0; x < 8; ++x) {
                    precompute[j][x] = (1 / sqrt(2)) * cos(0) * table.m_table[x][0][k][l];
                    for (int y = 1; y < 8; ++y) {
                        precompute[j][x] += cos(0.0625f * (j * 2 + 1) * y * pi) * table.m_table[x][y][k][l];
                    }
                }
            }
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    float resultValue = (1 / sqrt(2)) * cos(0) * precompute[j][0];
                    for (int x = 1; x < 8; ++x) {
                        // TODO computeCoefficientAtIndex can be precompute to table lookup
                        resultValue += cos(0.0625f * (i * 2 + 1) * x * pi) * precompute[j][x];
                    }
                    result.m_table[i][j][k][l] = resultValue * 0.25f;
                }
            }
        }
    }
}

void ImageBlock::FromComponentTable(const ComponentTable &table, int maxVerticalComponent, int maxHorizontalComponent) {
    m_table = new float *[8 * maxVerticalComponent];
    for (int i = 0; i < 8 * maxVerticalComponent; ++i) {
        m_table[i] = new float[8 * maxHorizontalComponent];
        for (int j = 0; j < 8 * maxHorizontalComponent; ++j) {
            int newI = i * table.m_verticalSize / maxVerticalComponent;
            int newJ = j * table.m_horizontalSize / maxHorizontalComponent;
            m_table[i][j] = table.m_table[newI % 8][newJ % 8][newI / 8][newJ / 8];
        }
    }
}

void ImageMCU::fromMCU(const JPEG &jpeg, const MCU &mcu) {
    for (int i = 0; i < jpeg.m_sof0.m_componentSize; ++i) {
        m_block[i].FromComponentTable(*mcu.m_component[i], jpeg.m_sof0.m_maxVerticalComponent,
                                      jpeg.m_sof0.m_maxHorizontalComponent);
    }
}

void Image::fromMCUS(const JPEG &jpeg, const MCUS &mcus) {
    m_mcuWidth = mcus.m_mcuWidth;
    m_mcuHeight = mcus.m_mcuHeight;
    m_imcu = new ImageMCU *[m_mcuHeight];
    for (int i = 0; i < m_mcuHeight; ++i) {
        m_imcu[i] = new ImageMCU[m_mcuWidth];
        for (int j = 0; j < m_mcuWidth; ++j) {
            m_imcu[i][j].fromMCU(jpeg, mcus.m_mcu[i][j]);
        }
    }
}

void Image::toPpm(std::ofstream &ofs, const JPEG &jpeg) {
    ofs << "P6\n";
    ofs << jpeg.m_sof0.m_width << " " << jpeg.m_sof0.m_height << "\n";
    ofs << 255 << "\n";
    if (!m_storedInBuffer) {
        m_componentSize = jpeg.m_sof0.m_componentSize;
        m_maxVerticalComponent = jpeg.m_sof0.m_maxVerticalComponent;
        m_maxHorizontalComponent = jpeg.m_sof0.m_maxHorizontalComponent;
        for (int i = 0; i < m_componentSize; ++i) {
            m_imageBuffer[i] = new float *[m_mcuHeight * 8 * m_maxVerticalComponent];
            for (int j = 0; j < m_mcuHeight * 8 * m_maxVerticalComponent; ++j) {
                m_imageBuffer[i][j] = new float[m_mcuWidth * 8 * m_maxHorizontalComponent];
            }
        }
        for (int i = 0; i < m_mcuHeight * 8 * m_maxVerticalComponent; ++i) {
            for (int j = 0; j < m_mcuWidth * 8 * m_maxHorizontalComponent; ++j) {
                const ImageMCU &imcu = m_imcu[i / (8 * m_maxVerticalComponent)][j / (8 * m_maxHorizontalComponent)];
                int tableI = i % (8 * m_maxVerticalComponent);
                int tableJ = j % (8 * m_maxHorizontalComponent);
                float y = imcu.m_block[0].m_table[tableI][tableJ];
                float cb = imcu.m_block[1].m_table[tableI][tableJ];
                float cr = imcu.m_block[2].m_table[tableI][tableJ];
                m_imageBuffer[Image::R_COMPONENT][i][j] = yCbCrConverter(Image::R_COMPONENT, y, cb, cr);
                m_imageBuffer[Image::G_COMPONENT][i][j] = yCbCrConverter(Image::G_COMPONENT, y, cb, cr);
                m_imageBuffer[Image::B_COMPONENT][i][j] = yCbCrConverter(Image::B_COMPONENT, y, cb, cr);
            }
        }
        m_storedInBuffer = true;
    }
    for (int i = 0; i < jpeg.m_sof0.m_height; ++i) {
        for (int j = 0; j < jpeg.m_sof0.m_width; ++j) {
            for (int k = 0; k < 3; ++k) {
                ofs << (clamp(m_imageBuffer[k][i][j]));
            }
        }
    }
}

float Image::yCbCrConverter(int component, float y, float cb, float cr) {
    if (component == Image::R_COMPONENT) {
        return y + 1.402f * cr + 128.0f;
    } else if (component == Image::G_COMPONENT) {
        return y - 0.71414f * cr - 0.34414f * cb + 128.0f;
    } else {
        return y + 1.772f * cb + 128.0f;
    }
}

uint8_t Image::clamp(float value) {
    if (value > 255) {
        return 255;
    } else if (value < 0) {
        return 0;
    } else {
        return (value - ((int) value) >= 0.5 ? value + 1 : value);
    }
}

Image::~Image() {
    for (int i = 0; i < m_mcuHeight; ++i) {
        delete[] m_imcu[i];
    }
    delete[] m_imcu;

    for (int i = 0; i < m_componentSize; ++i) {
        for (int j = 0; j < m_mcuHeight * 8 * m_maxVerticalComponent; ++j) {
            delete[] m_imageBuffer[i][j];
        }
        delete[] m_imageBuffer[i];
    }
}

void NaiveUpsampling::process(JPEG &jpeg) {
    jpeg.m_image = new Image();
    jpeg.m_image->fromMCUS(jpeg, jpeg.m_mcus);
}

Decoder &Decoder::setDequantization(IDequantization *dequantizationStrategy) {
    m_dequantization = dequantizationStrategy;
}

Decoder &Decoder::setDezigzag(IDezigzag *dezigzagStrategy) {
    m_dezigzag = dezigzagStrategy;
}

Decoder &Decoder::setIDCT(IIDCT *idctStrategy) {
    m_idct = idctStrategy;
}

Decoder &Decoder::setUpsampling(Upsampling *upsamplingStrategy) {
    m_upsampling = upsamplingStrategy;
}

void Decoder::precess(JPEG &jpeg) {
#ifdef DEBUG
    int lookI = 15, lookJ = 15;
    cout << "==== Before Process ====" << endl;
    cout << jpeg.m_mcus.m_mcu[lookI][lookJ];
#endif

    if (!m_dequantization) {
        cout << "[ERROR] Didn't assign dequantization strategy." << endl;
    }
    m_dequantization->process(jpeg);
#ifdef DEBUG
    cout << "==== After dequantization ====" << endl;
    cout << jpeg.m_mcus.m_mcu[lookI][lookJ];
#endif

    if (!m_dezigzag) {
        cout << "[ERROR] Didn't provide de ZIG-ZAG strategy." << endl;
    }
    m_dezigzag->process(jpeg);
#ifdef DEBUG
    cout << "==== After dezigzag ====" << endl;
    cout << jpeg.m_mcus.m_mcu[lookI][lookJ];
#endif

    if (!m_idct) {
        cout << "[ERROR] Didn't provide IDCT strategy." << endl;
    }
    m_idct->process(jpeg);
#ifdef DEBUG
    cout << "==== After idct ====" << endl;
    cout << jpeg.m_mcus.m_mcu[lookI][lookJ];
#endif

    if (!m_upsampling) {
        cout << "[ERROR] Didn't provide Upsampling strategy." << endl;
    }
    m_upsampling->process(jpeg);
}