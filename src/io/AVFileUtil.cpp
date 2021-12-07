//
// Created by bo on 2021/11/29.
//

#include "AVFileUtil.h"

uint8_t *AVFileUtil::readBinaryFile(const char *path, uint32_t &fileLength) {
    //read file
    std::ifstream ifstream;

    ifstream.open(path, std::ios::in | std::ios::binary);
    // read file to binary data
    ifstream.seekg(0,std::ios_base::end);
    uint32_t length = ifstream.tellg();
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

uint16_t *AVFileUtil::readBinaryFileAsUint16(const char *path, unsigned int &fileLength) {
    //read file
    std::ifstream ifstream;

    ifstream.open(path, std::ios::in | std::ios::binary);
    // read file to binary data
    ifstream.seekg(0,std::ios_base::end);
    unsigned int length = ifstream.tellg();
    if (length < 0) {
        return NULL;
    }
    ifstream.seekg(0, std::ios_base::beg);
    fileLength = length/sizeof (uint16_t);
    uint16_t * buffer = new uint16_t[length/sizeof (uint16_t)];
    ifstream.read((char *)buffer, length);
    //close file
    ifstream.close();

    if (isLittleEndien()) {
        for (int i = 0; i < fileLength; ++i) {
            buffer[i] = revertLittleToBig(buffer[i]);
        }
    }

    return buffer;
}

