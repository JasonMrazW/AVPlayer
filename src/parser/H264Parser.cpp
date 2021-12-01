//
// Created by bo on 2021/11/30.
//

#include "header/H264Parser.h"
#include "../io/AVFileUtil.h"
const char *h264_path = "resources/video/tmp.264";

void H264Parser::init() {
    unsigned int fileLength = 0;
    unsigned char *fileContent = AVFileUtil::readBinaryFile(h264_path,fileLength);

    std::cout << "file length : " << fileLength/(float)1000/1000 << "MB" << std::endl;

    unsigned char * temp = fileContent;

    int nalu_count = 0;

    int *temp_naluStartPosition = new int[fileLength / 1000];

    StartCodeType type;
    for (int i = 0; i < fileLength;) {
        type = getNALUType(temp);
        switch (type) {
            case Frame_3Byte:
                //3个字节后是NALU的Header
                i = i + 3;
                temp_naluStartPosition[nalu_count++] = i;
                break;
            case Frame_4Byte:
                //4个字节后是NALU的Header
                i = i + 4;
                temp_naluStartPosition[nalu_count++] = i;
                break;
            default:
                i++;
                break;
        }
        temp = &fileContent[i];
    }

    std::cout << "nalu count:" << nalu_count << std::endl;

    unsigned int *nalStartPoint = new unsigned int[nalu_count];

    // size = len * sizeof(data type)
    std::memcpy(nalStartPoint, temp_naluStartPosition, nalu_count * sizeof(unsigned int));

    // 构造NALU Struct
    int i = 0;
    NALU *nalu_array = new NALU[nalu_count];
    NALU *nalu_temp;
    unsigned int key_frame_count = 0;
    do {
        //计算NAL的起始位置，两个postion之间的长度
        unsigned int nalu_start_position = nalStartPoint[i];
        //std::cout << "position:" << nalu_start_position << std::endl;

        //init NAL_Header
        nalu_temp = new NALU();
        nalu_temp->header = new NALU_Header();
        nalu_array[i] = *nalu_temp;

        //read first byte
        loadHeader(fileContent, nalu_temp, nalu_start_position);
        //for test
        if (nalu_temp->header->nal_type == 5) {
            key_frame_count++;
        }

        //读第2个Byte到下一个NALU的起始字节，作为整个EBSP
        //指针指向NALU Header后第一个字节
        unsigned char * ebsp_pointer = (fileContent + nalu_start_position + 1);

        unsigned int ebsp_length = 0;
        if (i < nalu_count - 1) {//非最后一个nalu
            ebsp_length = nalStartPoint[i + 1] - nalu_start_position - 1;
        } else {//最后一个nalu
            ebsp_length = fileLength - nalu_start_position -1;
        }

        //TODO:如何校验rbsp数组内数据的正确性？
        unsigned char *tmp_rbsp = loadRBSP(ebsp_length, ebsp_pointer);

        

        i++;
    }while (i < nalu_count);

    std::cout << "key frame count:" << key_frame_count << std::endl;
    std::cout << "nalu count:" << nalu_count << std::endl;
}

unsigned char * H264Parser::loadRBSP(unsigned int ebsp_length, const unsigned char *ebsp_pointer) {//检测内部是否存在防竞争数据0x03，如果有就去除这部分数据
    //添加防竞争数据的情况：
    //0x000000---->0x00000300
    //0x000001---->0x00000301
    //0x000002---->0x00000302
    //0x000003---->0x00000303
    //一次遍历剔除指定元素
    unsigned char *temp_rbsp = new unsigned char[ebsp_length];

    int rbsp_index = 0;
    for (int j = 0; j < ebsp_length; ) {
        //检测第3个字节是否是0x03即可
        if (ebsp_pointer[j] == 0 && ebsp_pointer[j + 1] == 0 && ebsp_pointer[j + 2] == 0x03) {
            temp_rbsp[rbsp_index++] = ebsp_pointer[j];
            temp_rbsp[rbsp_index++] = ebsp_pointer[j + 1];
            //跳过0x03
            temp_rbsp[rbsp_index++] = ebsp_pointer[j + 3];
            j += 4;
        } else {
            temp_rbsp[rbsp_index++] = ebsp_pointer[j++];
        }
    }

    //过滤数组大小
    unsigned char *rbsp;
    if (rbsp_index < ebsp_length) {
        rbsp = new unsigned char[rbsp_index];
        memcpy(rbsp, temp_rbsp, rbsp_index * sizeof (char));
    } else {
        rbsp = temp_rbsp;
    }

    return rbsp;
}

/**
 * 加载头部数据
 * @param fileContent
 * @param nalu_temp
 * @param nalu_start_position
 */
void H264Parser::loadHeader(const unsigned char *fileContent, H264Parser::NALU *nalu_temp, unsigned int nalu_start_position) {
    //---1bit-------2bit-----5bit---
    //---forbidden---ref_id---type---
    unsigned char first_byte = fileContent[nalu_start_position];
    nalu_temp->header->forbidden = (unsigned char )(first_byte & 0x80) >> 7;
    nalu_temp->header->nal_ref_idc = (unsigned char )(first_byte & 0x60) >> 5;
    nalu_temp->header->nal_type = fileContent[nalu_start_position] & 0x1F;

    //std::cout << "nal header " << nalu_temp->header->forbidden << "," << nalu_temp->header->nal_ref_idc << "," << nalu_temp->header->nal_type << std::endl;
}


H264Parser::H264Parser() {

}

H264Parser::~H264Parser() {

}

H264Parser::StartCodeType H264Parser::getNALUType(const unsigned char *buf) {
    //先判断是否符合3个字节
    if ((*buf == 0x00) && (*(buf+1) == 0x00) && (*(buf+2) == 0x01)) {
        return H264Parser::Frame_3Byte;
    } else if(*buf == 0x00 && *(buf+1) == 0x00 && *(buf+2) == 0x00 &&  *(buf+3) == 0x01) {
        return H264Parser::Frame_4Byte;
    } else {
        return other;
    }
}
