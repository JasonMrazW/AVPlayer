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

    int *positionArray = new int[fileLength/1000];
    int currentPosition = 0;

    for (int i = 0; i < fileLength;) {
       StartCodeType type = getNALUType(temp);
        switch (type) {
            case Frame_3Byte:
                //3个字节后是NALU的Header
                i = i + 3;
                positionArray[currentPosition++] = i;
                naluCount++;
                break;
            case Frame_4Byte:
                //4个字节后是NALU的Header
                i = i + 4;
                positionArray[currentPosition++] = i;
                naluCount++;
                break;
            default:
                i++;
                break;
        }
        temp = &fileContent[i];
    }

    std::cout << "currentPosition:" << currentPosition << std::endl;

    int *nalStartPoint = new int[currentPosition];

    // size = len * sizeof(data type)
    std::memcpy(nalStartPoint, positionArray, currentPosition * sizeof(int));

    // 构造NAL 对象
    int i = 0;
    NALU *naluArray = new NALU[sizeof (NALU) * naluCount];
    NALU *nalu_temp;
    int key_frame_count = 0;
    do {
        //计算NAL的起始位置，两个postion之间的长度
        int nal_startPosition = nalStartPoint[i];
        std::cout << "position:" << nal_startPosition << std::endl;

        //init NAL_Header
        nalu_temp = new NALU();
        nalu_temp->header = new NALU_Header();
        naluArray[i] = *nalu_temp;

        //read first byte
        //---1bit-------2bit-----5bit---
        //---forbidden---ref_id---type---
        unsigned char first_byte = fileContent[nal_startPosition];
        nalu_temp->header->forbidden = (unsigned char )(first_byte & 0x80) >> 7;
        nalu_temp->header->nal_ref_idc = (unsigned char )(first_byte & 0x60) >> 5;
        nalu_temp->header->nal_type = fileContent[nal_startPosition] & 0x1F;

        if (nalu_temp->header->nal_type == 5) {
            key_frame_count++;
        }

        std::cout << "nal header " << nalu_temp->header->forbidden << "," << nalu_temp->header->nal_ref_idc << "," << nalu_temp->header->nal_type << std::endl;
        i++;
    }while (i < naluCount);

    std::cout << "key frame count:" << key_frame_count << std::endl;
    std::cout << "nalu count:" << naluCount << std::endl;
}


H264Parser::H264Parser() {

}

H264Parser::~H264Parser() {

}

H264Parser::StartCodeType H264Parser::getNALUType(const char *buf) {
    //先判断是否符合3个字节
    if ((*buf == 0x00) && (*(buf+1) == 0x00) && (*(buf+2) == 0x01)) {
        return H264Parser::Frame_3Byte;
    } else if(*buf == 0x00 && *(buf+1) == 0x00 && *(buf+2) == 0x00 &&  *(buf+3) == 0x01) {
        return H264Parser::Frame_4Byte;
    } else {
        return other;
    }
}
