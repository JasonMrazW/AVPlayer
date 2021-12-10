//
// Created by bo on 2021/12/7.
//

#ifndef AVPLAYER_FLVPARASER_H
#define AVPLAYER_FLVPARASER_H
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "FLVAMFInfo.h"
#include "FLVAudioTag.h"

//----24bit----1bit----5bit----1bit----1bit----1bit----32bit----
//----FLV------1-------00000---0 or 1--0-------0 or 1--0x0009---
typedef struct FLV_Header {
    uint8_t *signature; // 签名，24bit，固定为FLV
    uint8_t version; //1bit
    bool hasVideo;
    bool hasAudio;
    uint32_t header_size; //header size, 32bit, 始终4个字节
} FLVHeader;

typedef struct FLV_Tag {
    uint8_t tag_type;
    uint32_t data_size; //data部分的数据长度
    uint32_t timestamp; //tag相对第一个tag的时间戳
    uint32_t stream_id; //固定为3个字节的0x000000
    uint8_t *data;//数据
    void    *data_object; //具体的data类型
} FLVTag;

enum TAG_TYPE {
    AUDIO = 8,
    VIDEO = 9,
    SCRIPT = 18,
    OTHERS
};

class FLVParaser {
public:
    FLVParaser();

    ~FLVParaser();

    static void init();

    static void parserHeader(const unsigned char *file_content);

    static FLVTag *parseFLVTag(uint32_t file_length, uint8_t *file_content, uint16_t &flv_tag_count);

    static uint32_t parseScriptData(uint8_t *script_data,  ScriptDataValue *data_value);

    static uint32_t readScriptString(uint8_t *script_data, ScriptString &scriptString);

    static uint32_t readScriptDataObject(uint8_t *script_data, ScriptDataObject &dataObject);

    static void parseAudioTag(uint8_t *audio_data);
};


#endif //AVPLAYER_FLVPARASER_H
