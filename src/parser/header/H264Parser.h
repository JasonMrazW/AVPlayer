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
    void init();

    StartCodeType getNALUType(char * buf);
};


#endif //AVPLAYER_H264PARSER_H
