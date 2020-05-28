//
// Created by Edge on 2020/5/26.
//

#ifndef JPEG_CODEC_DECODER_H
#define JPEG_CODEC_DECODER_H

#include "Segment.h"

class IDequantization {
public:
    virtual void process(JPEG &jpeg) = 0;
};

class NaiveDequantization : public IDequantization {
public:
    void process(JPEG &jpeg) override;
};

class IDezigzag {
public:
    virtual void process(JPEG &jpeg) = 0;
};

class NaiveDezigzag : public IDezigzag {
public:
    void process(JPEG &jpeg) override;
};

class EnhancedDezigzag : public IDezigzag {
public:
    void process(JPEG &jpeg) override;
};

class IIDCT {
public:
    virtual void process(JPEG &jpeg) = 0;
protected:
    float coefficientPrecompute(int x, int y);
};

class NaiveIDCT : public IIDCT {
public:
    void process(JPEG &jpeg) override;

private:
    void performIdctOnComponentTable(ComponentTable &table, ComponentTable &result);

    float computeCoefficientAtIndex(ComponentTable &table, int verticalComponent, int horizonComponent, int i, int j);

};

class DimensionReductionIDCT : public IIDCT {
public:
    void process(JPEG &jpeg) override;

private:
    static void performIdctOnComponentTable(ComponentTable &table, ComponentTable &result);

};

class ImageBlock {
public:
    ImageBlock(): m_table(nullptr) {};
    void FromComponentTable(const ComponentTable &table, int maxVerticalComponent, int maxHorizontalComponent);

    float **m_table;
};

class ImageMCU {
public:
    void fromMCU(const JPEG &jpeg, const MCU &mcu);

    ImageBlock m_block[4];

};

class Image {
public:
    Image() : m_imcu(nullptr), m_imageBuffer{}, m_storedInBuffer(false) {};
    ~Image();
    void fromMCUS(const JPEG &jpeg, const MCUS &mcus);

    void toPpm(std::ofstream &ofs, const JPEG &jpeg);
    static float yCbCrConverter(int component, float y, float cb, float cr);
    static uint8_t clamp(float value);

    int m_mcuWidth, m_mcuHeight;
    int m_componentSize;
    int m_maxVerticalComponent, m_maxHorizontalComponent;
    ImageMCU **m_imcu;
    float **m_imageBuffer[3];
    bool m_storedInBuffer;

    static constexpr int R_COMPONENT = 0;
    static constexpr int G_COMPONENT = 1;
    static constexpr int B_COMPONENT = 2;
};

class Upsampling {
public:
    virtual void process(JPEG &jpeg) = 0;
};

class NaiveUpsampling : public Upsampling {
public:
    void process(JPEG &jpeg) override;
};

class Decoder {
public:
    Decoder() : m_dequantization(nullptr), m_dezigzag(nullptr) {};

    Decoder &setDequantization(IDequantization *dequantizationStrategy);

    Decoder &setDezigzag(IDezigzag *dezigzagStrategy);

    Decoder &setIDCT(IIDCT *idctStrategy);

    Decoder &setUpsampling(Upsampling *upsamplingStrategy);

    void precess(JPEG &jpeg);

private:
    IDequantization *m_dequantization;
    IDezigzag *m_dezigzag;
    IIDCT *m_idct;
    Upsampling *m_upsampling;
};


#endif //JPEG_CODEC_DECODER_H
