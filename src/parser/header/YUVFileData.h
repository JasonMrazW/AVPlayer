//
// Created by bo on 2021/11/29.
//

#ifndef AVPLAYER_YUVFILEDATA_H
#define AVPLAYER_YUVFILEDATA_H


#include <SDL_stdinc.h>

class YUVFileData {
public:
    uint8_t *data; // 文件二进制内容
    int width; //分辨率宽
    int height; //分辨率高
    Uint32 format; //文件格式
    int pin;
};


#endif //AVPLAYER_YUVFILEDATA_H
