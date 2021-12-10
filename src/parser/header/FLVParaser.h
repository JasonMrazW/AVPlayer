//
// Created by bo on 2021/12/7.
//

#ifndef AVPLAYER_FLVPARASER_H
#define AVPLAYER_FLVPARASER_H
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "FLVStructDefine.h"


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
};


#endif //AVPLAYER_FLVPARASER_H
