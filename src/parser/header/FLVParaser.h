//
// Created by bo on 2021/12/7.
//

#ifndef AVPLAYER_FLVPARASER_H
#define AVPLAYER_FLVPARASER_H
#include <cstdlib>

//----24bit----1bit----5bit----1bit----1bit----1bit----32bit----
//----FLV------1-------00000---0 or 1--0-------0 or 1--0x0009---
typedef struct FLV_Header {
    uint8_t *signature; // 签名，24bit，固定为FLV
    uint8_t version; //1bit
    bool hasVideo;
    bool hasAudio;
    uint32_t header_size; //header size, 32bit, 始终4个字节
} FLVHeader;


class FLVParaser {
public:
    FLVParaser();
    ~FLVParaser();

    static void init();

    static void parserHeader(const unsigned char *file_content);
};


#endif //AVPLAYER_FLVPARASER_H
