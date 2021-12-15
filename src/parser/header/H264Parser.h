//
// Created by bo on 2021/11/30.
//

#ifndef AVPLAYER_H264PARSER_H
#define AVPLAYER_H264PARSER_H
#include <iostream>
#include <fstream>

enum NALU_TYPE {
    //0~23为真实的NALU类型

    //一个PACKET包含多个NALU
    NALU_TYPE_STAP_A = 24,
    NALU_TYPE_STAP_B = 25,
    NALU_TYPE_MTAP_16 = 26,
    NALU_TYPE_MTAP_24 = 27,
    //多个PACKET包含一个NALU
    NALU_TYPE_FU_A = 28,
    NALU_TYPE_FU_B = 29
};

class H264Parser {
public:
    H264Parser();
    ~H264Parser();

    enum StartCodeType {
        Frame_4Byte, //4个字节的起始码，接下来是一帧的数据开始
        Frame_3Byte, //3个字节的起始码，接下里是一帧的部分数据
        other
    };

    struct SODB {

    };
    struct NALU_Header {
        int forbidden; //禁止位，默认是false
        int nal_ref_idc; //优先级位
        int nal_type ; //nal 数据类型
    };
    struct RBSP {
        SODB *sodb;
    };
    struct NALU {
        NALU_Header *header;
        RBSP *rbsp;
    };
    enum LOAD_FROM {
        FILE,
        STREAM
    };

    void loadFromFile();

    void loadFromStream(uint8_t *data_content, uint32_t data_length);

    void loadNALU(uint8_t *data_content, uint32_t data_length);

    static StartCodeType getNALUType(const unsigned char * buf);

    static void loadHeader(const unsigned char *fileContent, NALU *nalu_temp, unsigned int nalu_start_position) ;

    static unsigned char * loadRBSP(unsigned int ebsp_length, const unsigned char *ebsp_pointer, uint32_t &rbsp_length);

    static unsigned char *  loadSODB(unsigned char *tmp_rbsp, uint32_t rbsp_length, uint32_t &sodb_length);
};


#endif //AVPLAYER_H264PARSER_H
