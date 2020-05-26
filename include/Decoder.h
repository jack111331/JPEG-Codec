//
// Created by Edge on 2020/5/26.
//

#ifndef JPEG_CODEC_DECODER_H
#define JPEG_CODEC_DECODER_H

#include "Segment.h"

class Dequantization {
public:
    virtual void process(JPEG &jpeg) = 0;
};

class NaiveDequantization : public Dequantization {
public:
    void process(JPEG &jpeg) override;
};

class Dezigzag {
public:
    virtual void process(JPEG &jpeg) = 0;
};

class NaiveDezigzag : public Dezigzag {
public:
    void process(JPEG &jpeg) override;
};

class EnhancedDezigzag : public Dezigzag {
public:
    void process(JPEG &jpeg) override;
};

class IIDCT {
public:
    virtual void process(JPEG &jpeg) = 0;
};

class IDCT : public IIDCT {
public:
    void process(JPEG &jpeg) override;
};

class Decoder {
public:
    Decoder(): m_dequantization(nullptr), m_dezigzag(nullptr) {};
    Decoder &setDequantization(Dequantization *dequantizationStrategy);
    Decoder &setDezigzag(Dezigzag *dezigzagStrategy);
    Decoder &setIDCT(IIDCT *idctStrategy);
    void precess(JPEG &jpeg);
private:
    Dequantization *m_dequantization;
    Dezigzag *m_dezigzag;
    IIDCT *m_idct;
};


#endif //JPEG_CODEC_DECODER_H
