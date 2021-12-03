//
// Created by bo on 2021/12/3.
//

#include "BinaryUtil.h"

//两个char合并成一个有符号的short
uint16_t BinaryUtil::getUint16(uint8_t high, uint8_t low) {
    return (short)((high<< 8)|(low & 0xFF));
}
