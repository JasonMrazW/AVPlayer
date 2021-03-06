//
// Created by bo on 2021/11/29.
//

#ifndef AVPLAYER_IIMAGEPARSER_H
#define AVPLAYER_IIMAGEPARSER_H
#include "YUVFileData.h"
#include <cstdlib>
#include <iostream>
#include <fstream>

class IImageParser {
public:
    IImageParser() {

    }

    ~IImageParser() {
        yuvFileData = NULL;
    }

    YUVFileData *yuvFileData;

    virtual YUVFileData* loadFile() {
        return NULL;
    }

    virtual YUVFileData *init() = 0;

    /**
     * 加载二进制数据，返回char*
     * @param yuvFilePath
     * @param width
     * @param height
     * @return
     */
    static uint8_t * loadFile(const char *yuvFilePath, int width, int height) {
        //read file
        std::ifstream ifstream;
        ifstream.open(yuvFilePath, std::ios::in | std::ios::binary);
        // read yuv file to binary data
        // default yuv420p
        ifstream.seekg(0,std::ios_base::end);
        int length = ifstream.tellg();
        if (length < 0) {
            return NULL;
        }
        ifstream.seekg(0, std::ios_base::beg);
        int frameSize = width * height * 3/2;
        uint8_t* buffer = new uint8_t[length];
        ifstream.read(reinterpret_cast<char *>(buffer), length);
        //close file
        ifstream.close();

        return buffer;
    }
};

#endif //AVPLAYER_IIMAGEPARSER_H
