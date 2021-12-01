//
// Created by bo on 2021/11/29.
//

#include "AVFileUtil.h"

unsigned char *AVFileUtil::readBinaryFile(const char *path, int &fileLength) {
    //read file
    std::ifstream ifstream;

    ifstream.open(path, std::ios::in | std::ios::binary);
    // read file to binary data
    ifstream.seekg(0,std::ios_base::end);
    int length = ifstream.tellg();
    if (length < 0) {
        return NULL;
    }
    ifstream.seekg(0, std::ios_base::beg);
    unsigned char* buffer = new unsigned char[length];
    ifstream.read(reinterpret_cast<char *>(buffer), length);
    //close file
    ifstream.close();

    fileLength = length;

    return buffer;
}
