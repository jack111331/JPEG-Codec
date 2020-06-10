//
// Created by Edge on 2020/5/11.
//

#include "Segment.h"
#include <iostream>
#include <Decoder.h>
#include <string>
//#define DEBUG

using namespace std;

string image_list[1] = {
//        "img/gig-sn01.jpg",
        "img/gig-sn08.jpg"//,
//        "img/monalisa.jpg",
//        "img/teatime.jpg"
};

int main() {
    for(int i = 0;i < 1;++i) {
        JPEG data;
        Decoder decoder = Decoder().setDequantization(new NaiveDequantization()).setDezigzag(new EnhancedDezigzag()).setIDCT(
                new DimensionReductionIDCT()).setUpsampling(new NaiveUpsampling());

        ifstream ifs(image_list[i], std::ios::binary);
        if (ifs.is_open()) {
            ifs >> data;
            ifs.close();
            decoder.precess(data);
            ofstream ofs(image_list[i].substr(0, image_list[i].find("."))+".ppm", std::ios::binary);
            data.m_image->toPpm(ofs, data);
            ofs.close();
            data.m_image->saveToBmp(image_list[i].substr(0, image_list[i].find("."))+".bmp", data);
        }
    }

}