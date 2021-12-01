//
// Created by bo on 2021/11/30.
//

#ifndef AVPLAYER_H264PARSER_H
#define AVPLAYER_H264PARSER_H
#include <iostream>
#include <fstream>

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
    void init();

    static StartCodeType getNALUType(const unsigned char * buf);

    static void loadHeader(const unsigned char *fileContent, NALU *nalu_temp, int nalu_start_position) ;

    unsigned char * loadRBSP(int ebsp_length, const unsigned char *ebsp_pointer);
};


#endif //AVPLAYER_H264PARSER_H
