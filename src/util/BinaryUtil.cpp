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

uint64_t BinaryUtil::getUint64(uint8_t *content, uint8_t size) {
    uint64_t result = 0;
    uint8_t total = (size-1) * 8;
    for (int i = 0; i < size; ++i) {
        result |= (uint64_t)((content[i] & 0xFF) << (total-i*8));
        std::cout << "i-----:" << ((uint64_t)((content[i] & 0xFF) << (total-i*8))) << "||||" << result  << "..."<< std::endl;
    }
    return result;
}

uint8_t *BinaryUtil::getUint8(uint8_t *content, uint16_t size) {
    uint8_t *result = new uint8_t [size];
    memcpy(result, content, size);
    return result;
}

double BinaryUtil::getDouble(uint8_t *content, uint8_t size) {
    double result = stringToDouble(content, size);
    return result;
}

uint8_t BinaryUtil::ReadByte(uint8_t *data) {
    return static_cast<uint8_t>(data[0]);
}


uint64_t BinaryUtil::Read8Bytes(uint8_t *data) {
    uint64_t offset1 = static_cast<uint64_t>(ReadByte(data)) << 56;
    uint64_t offset2 = static_cast<uint64_t>(ReadByte(data + 1)) << 48;
    uint64_t offset3 = static_cast<uint64_t>(ReadByte(data + 2)) << 40;
    uint64_t offset4 = static_cast<uint64_t>(ReadByte(data + 3)) << 32;
    uint64_t offset5 = static_cast<uint64_t>(ReadByte(data + 4)) << 24;
    uint64_t offset6 = static_cast<uint64_t>(ReadByte(data + 5)) << 16;
    uint64_t offset7 = static_cast<uint64_t>(ReadByte(data + 6)) << 8;
    uint64_t offset8 = static_cast<uint64_t>(ReadByte(data + 7));
    return static_cast<uint64_t>(offset1 | offset2 | offset3 | offset4 |
    offset5 | offset6 | offset7 | offset8);
}

// string to double value
double BinaryUtil::stringToDouble(uint8_t *hex, uint32_t length)
{
    double result = 0.0;
    char hexStr[length * 2];
    memset(hexStr, 0, sizeof(hexStr));
    for (uint32_t i = 0; i < length; i++) {
        sprintf(hexStr + i * 2, "%02x", hex[i]);
    }
    sscanf(hexStr, "%llx", &result);
    return result;
}


