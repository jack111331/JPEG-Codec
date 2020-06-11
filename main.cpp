//
// Created by Edge on 2020/5/11.
//

//#define DEBUG

#include "Segment.h"
#include <iostream>
#include <Decoder.h>
#include <string>

using namespace std;

int main(int argc, char **argv) {
    string inputFile;
    string outputFile;
    for (int i = 1; i < argc; ++i) {
        string cmd(argv[i++]);
        if (cmd == "-i") {
            inputFile = argv[i];
        } else if (cmd == "-o") {
            outputFile = argv[i];
        }
    }
    if (inputFile.empty()) {
        if (argc >= 2) {
            inputFile = argv[1];
        } else {
            std::cout << "[ERROR] input file is empty" << std::endl;
            exit(1);
        }
    }
    JPEG data;
    // Setup decode strategy
    Decoder decoder = Decoder().setDequantization(new NaiveDequantization()).setDezigzag(
                    new EnhancedDezigzag()).setIDCT(new DimensionReductionIDCT()).setUpsampling(new NaiveUpsampling());

    ifstream ifs(inputFile, std::ios::binary);
    if (ifs.is_open()) {
        ifs >> data;
        ifs.close();
        decoder.process(data);
        if (!outputFile.empty()) {
            data.m_image->saveToBmp(outputFile, data);
        } else {
            data.m_image->saveToBmp(inputFile.substr(0, inputFile.find(".")) + ".bmp", data);
        }
    }

}