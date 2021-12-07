//
// Created by bo on 2021/11/29.
//

#ifndef AVPLAYER_AVFILEUTIL_H
#define AVPLAYER_AVFILEUTIL_H
#include <iostream>
#include <fstream>

#define revertLittleToBig(A) ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))

class AVFileUtil {
public:
    static uint8_t * readBinaryFile(const char *path, uint32_t &fileLength);

    static uint16_t * readBinaryFileAsUint16(const char *path, uint32_t &fileLength);

    /**
     * 判断是否小端
     * @return
     */
    static bool isLittleEndien() {
        int i = 1;
        char *p = (char *)&i;
        if (*p == 1) {
            return true;
        }
        return false;
    };
};


#endif //AVPLAYER_AVFILEUTIL_H
