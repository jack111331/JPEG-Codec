//
// Created by Edge on 2020/5/11.
//

#include "Segment.h"
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    JPEG data;
    ifstream ifs("img/gig-sn01.jpg", std::ios::binary);
    if(ifs.is_open()) {
        ifs >> data;
        cout << data;
        ifs.close();
    }
}