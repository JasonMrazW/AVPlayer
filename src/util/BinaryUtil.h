//
// Created by bo on 2021/12/3.
//

#ifndef AVPLAYER_BINARYUTIL_H
#define AVPLAYER_BINARYUTIL_H
#include <cstdlib>

class BinaryUtil {
public:
    static uint16_t getUint16(uint8_t high, uint8_t low);

    static uint32_t getUint32(uint8_t *content, uint8_t size);
};


#endif //AVPLAYER_BINARYUTIL_H
