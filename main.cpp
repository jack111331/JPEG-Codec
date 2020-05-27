//
// Created by Edge on 2020/5/11.
//

#include "Segment.h"
#include <iostream>
#include <Decoder.h>

//#define DEBUG

using namespace std;

int main() {
    JPEG data;
    Decoder decoder = Decoder().setDequantization(new NaiveDequantization()).setDezigzag(new NaiveDezigzag()).setIDCT(
            new NaiveIDCT()).setUpsampling(new NaiveUpsampling());
    ifstream ifs("img/gig-sn01.jpg", std::ios::binary);
    if (ifs.is_open()) {
        ifs >> data;
        ifs.close();
        decoder.precess(data);
        ofstream ofs("img/gig-sn01.ppm", std::ios::binary);
        data.m_image->toPpm(ofs, data);
        ofs.close();
    }
}