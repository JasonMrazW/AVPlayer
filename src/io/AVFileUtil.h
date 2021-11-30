//
// Created by bo on 2021/11/29.
//

#ifndef AVPLAYER_AVFILEUTIL_H
#define AVPLAYER_AVFILEUTIL_H
#include <iostream>
#include <fstream>

class AVFileUtil {
public:
    static char* readBinaryFile(const char *path,int &fileLength);
};


#endif //AVPLAYER_AVFILEUTIL_H
