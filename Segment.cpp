//
// Created by Edge on 2020/5/11.
//

#include <iostream>
#include <Utility.h>
#include "Segment.h"

using std::ifstream;
using std::cout;
using std::endl;

std::ifstream &operator>>(std::ifstream &ifs, APP0 &data) {
    if (!checkData(ifs, APP0::MARKER_MAGIC_NUMBER, sizeof(APP0::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] APP0 marker mismatch." << endl;
        exit(1);
    }
    uint16_t length;
    ifs >> length;
    return ifs;
}

std::ifstream &operator>>(std::ifstream &ifs, JPEG &data) {
    if (!checkData(ifs, JPEG::MARKER_MAGIC_NUMBER, sizeof(JPEG::MARKER_MAGIC_NUMBER))) {
        cout << "[ERROR] JPEG marker mismatch." << endl;
        exit(1);
    }
    ifs >> data.m_app0;
}