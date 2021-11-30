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

    char * temp = fileContent;

    int naluCount = 0;
    for (int i = 0; i < fileLength; ++i) {
       StartCodeType type = getNALUType(temp);
        switch (type) {
            case Frame_3Byte:
            case Frame_4Byte:
                naluCount++;
                break;
        }
        temp = &fileContent[i];
    }
    std::cout << "nalu count:" << naluCount << std::endl;
}


H264Parser::H264Parser() {

}

H264Parser::~H264Parser() {

}

H264Parser::StartCodeType H264Parser::getNALUType(char *buf) {
    //先判断是否符合3个字节
    if ((*buf == 0x00) && (*(buf+1) == 0x00) && (*(buf+2) == 0x01)) {
        return H264Parser::Frame_3Byte;
    } else if(*buf == 0x00 && *(buf+1) == 0x00 && *(buf+2) == 0x00 &&  *(buf+3) == 0x01) {
        return H264Parser::Frame_4Byte;
    } else {
        return other;
    }

}
