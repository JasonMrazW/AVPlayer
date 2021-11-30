//
// Created by bo on 2021/11/30.
//

#include "header/H264Parser.h"
#include "../io/AVFileUtil.h"
const char *h264_path = "resources/video/tmp.264";

void H264Parser::init() {
    int fileLength = 0;
    char *fileContent = AVFileUtil::readBinaryFile(h264_path,fileLength);

    std::cout << "file length : " << fileLength/(float)1000/1000 << "MB" << std::endl;

    for (int i = 0; i < 50; ++i) {
        std::cout << std::hex << (int)fileContent[i] << std::dec << ",";
    }
}

H264Parser::H264Parser() {

}

H264Parser::~H264Parser() {

}
