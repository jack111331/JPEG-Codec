//
// Created by Edge on 2020/5/11.
//

#include "Segment.h"
#include <fstream>
#include <iostream>
#include <Decoder.h>

using namespace std;

int main() {
    JPEG data;
    Decoder decoder = Decoder().setDequantization(new NaiveDequantization()).setDezigzag(new NaiveDezigzag()).setIDCT(
            new IDCT()).setUpsampling(new NaiveUpsampling());
    ifstream ifs("img/gig-sn01.jpg", std::ios::binary);
    if (ifs.is_open()) {
        ifs >> data;
        ifs.close();
        decoder.precess(data);
        ofstream ofs("img/gig-sn01.ppm");
        data.m_image->toPpm(ofs, data);
        ofs.close();
    }
}