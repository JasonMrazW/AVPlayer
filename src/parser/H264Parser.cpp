//
// Created by bo on 2021/11/30.
//

#include "header/H264Parser.h"
#include "../io/AVFileUtil.h"
const char *h264_path = "resources/video/tmp.264";
using namespace std;


void H264Parser::loadFromFile() {
    uint32_t fileLength = 0;
    uint8_t *fileContent = AVFileUtil::readBinaryFile(h264_path, fileLength);
    loadNALU(fileContent, fileLength);
}

void H264Parser::loadFromStream(uint8_t *data_content, uint32_t data_length) {
    loadNALU(data_content, data_length);
}

void H264Parser::loadNALU(uint8_t *data_content, uint32_t data_length) {
    std::cout << "data length : " << data_length / (float)1000 / 1000 << "MB" << std::endl;

    unsigned char * temp = data_content;

    int nalu_count = 0;

    int *temp_naluStartPosition = new int[data_length / 1000];
    uint32_t *temp_naluLengthPosition = new uint32_t[data_length / 1000];

    StartCodeType type;
    for (int i = 0; i < data_length;) {
        type = getNALUType(temp);
        switch (type) {
            case Frame_3Byte:
                //3个字节后是NALU的Header
                i = i + 3;
                if (nalu_count >= 1) {
                    temp_naluLengthPosition[nalu_count-1] = i-3-temp_naluStartPosition[nalu_count-1];
                }
                temp_naluStartPosition[nalu_count++] = i;
                break;
                case Frame_4Byte:
                    //4个字节后是NALU的Header
                    i = i + 4;
                    if (nalu_count >= 1) {
                        temp_naluLengthPosition[nalu_count-1] = i-4-temp_naluStartPosition[nalu_count-1];
                    }
                    temp_naluStartPosition[nalu_count++] = i;
                    break;
                    default:
                        i++;
                        break;
        }
        temp = &data_content[i];
    }
    temp_naluLengthPosition[nalu_count-1] = data_length - temp_naluStartPosition[nalu_count - 1];

    std::cout << "nalu count:" << nalu_count << std::endl;

    unsigned int *nalStartPoint = new unsigned int[nalu_count];

    // size = len * sizeof(data channel_type)
    std::memcpy(nalStartPoint, temp_naluStartPosition, nalu_count * sizeof(unsigned int));

    // 构造NALU Struct
    int i = 0;
    NALU *nalu_array = new NALU[nalu_count]; // delete[] nalu_array
    NALU *nalu_temp;
    unsigned int key_frame_count = 0;
    do {
        //计算NAL的起始位置，两个postion之间的长度
        unsigned int nalu_start_position = nalStartPoint[i];
        //std::cout << "position:" << nalu_start_position << std::endl;

        //init NAL_Header
        nalu_temp = new NALU(); // delete nalu_tmp;
        nalu_temp->header = new NALU_Header();
        nalu_array[i] = *nalu_temp;

        //read first byte
        loadHeader(data_content, nalu_temp, nalu_start_position);
        //for test
        if (nalu_temp->header->nal_type == 5) {
            key_frame_count++;
        }

        //读第2个Byte到下一个NALU的起始字节，作为整个EBSP
        //指针指向NALU Header后第一个字节
        unsigned char * ebsp_pointer = (data_content + nalu_start_position + 1);

        unsigned int ebsp_length = temp_naluLengthPosition[i] - 1;

        //printf("ebsp'last byte：%02hhx", ebsp_pointer[ebsp_length-1]);
        //printf("\r\n");
        //TODO:如何校验rbsp数组内数据的正确性？
        uint32_t rbsp_length = 0;
        unsigned char *tmp_rbsp = loadRBSP(ebsp_length, ebsp_pointer, rbsp_length);
        //std::cout << "nalu channel_type:" << nalu_temp->fixed_header->nal_type << " ";
        //std::cout << "rbsp length:" << rbsp_length << std::endl;

        uint32_t  sodb_length = 0;
        unsigned char *tmp_sodb = loadSODB(tmp_rbsp, rbsp_length, sodb_length);
        //std::cout << "sodb length:" << sodb_length << std::endl;

        i++;
    }while (i < nalu_count);

    std::cout << "key frame count:" << key_frame_count << std::endl;
}



unsigned char *  H264Parser::loadSODB(unsigned char *tmp_rbsp, uint32_t rbsp_length, uint32_t &sodb_length) {
    //从尾部向前遍历
    //可能存在补齐，也可能不存在补齐的逻辑
    uint8_t tmp;
    uint8_t factor;
    tmp = tmp_rbsp[rbsp_length-1];

    if ((tmp) != 0) {
        //找到具体是哪位非0
        factor = 0x01;
        while ((tmp & factor) == 0 && tmp >= 0) {
            tmp = tmp >> 1;
        }

        tmp = tmp >> 1;
    }

    unsigned char * sodb;
    if (tmp >= 0) { //找到了，对应的位置是factor
        //i-1前的数据保留，同时第i个字节的tmp即为处理后的结果
        sodb = new unsigned char [rbsp_length];
        memcpy(sodb, tmp_rbsp, rbsp_length-1);
        //对tmp做处理，仅保留factor前的数据
        sodb[rbsp_length-1] = tmp;
        sodb_length = rbsp_length;
        if (tmp > 0) {
            //std::cout << "tmp：" << (int)tmp << endl;
        }
    } else {
        sodb = tmp_rbsp;
        sodb_length = rbsp_length;
    }

    return sodb;
}

unsigned char * H264Parser::loadRBSP(unsigned int ebsp_length, const unsigned char *ebsp_pointer, uint32_t &rbsp_length) {//检测内部是否存在防竞争数据0x03，如果有就去除这部分数据
    //添加防竞争数据的情况：
    //0x000000---->0x00000300
    //0x000001---->0x00000301
    //0x000002---->0x00000302
    //0x000003---->0x00000303
    //一次遍历剔除指定元素
    unsigned char *temp_rbsp = new unsigned char[ebsp_length];

    uint32_t rbsp_index = 0;
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
        memcpy(rbsp, temp_rbsp, rbsp_index);
    } else {
        rbsp = temp_rbsp;
    }

    rbsp_length = rbsp_index;

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
    //---forbidden---ref_id---channel_type---
    unsigned char first_byte = fileContent[nalu_start_position];
    nalu_temp->header->forbidden = (unsigned char )(first_byte & 0x80) >> 7;
    nalu_temp->header->nal_ref_idc = (unsigned char )(first_byte & 0x60) >> 5;
    nalu_temp->header->nal_type = fileContent[nalu_start_position] & 0x1F;

    //std::cout << "nal fixed_header " << nalu_temp->fixed_header->forbidden << "," << nalu_temp->fixed_header->nal_ref_idc << "," << nalu_temp->fixed_header->nal_type << std::endl;
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

