//
// Created by bo on 2021/12/3.
//

#include "BinaryUtil.h"

//两个char合并成一个有符号的short
uint16_t BinaryUtil::getUint16(uint8_t high, uint8_t low) {
    return (short)((high<< 8)|(low & 0xFF));
}

uint32_t BinaryUtil::getUint32(uint8_t *content, uint8_t size) {
    uint32_t result = 0;
    uint8_t total = (size-1) * 8;
    for (int i = 0; i < size; ++i) {
        result |= (content[i] & 0xFF) << (total-i*8);
    }
    return result;
}
