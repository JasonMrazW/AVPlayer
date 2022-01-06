//
// Created by bo on 2022/1/6.
//

#ifndef AVPLAYER_FFLOGER_H
#define AVPLAYER_FFLOGER_H
#include <iostream>
extern "C" {
#include "libavutil/avutil.h"
};
class FFLoger {
public:
    static void printErrorMessage(std::string error_prefix, int error_code) {
        char error[1024] = {0};
        av_strerror(error_code, error, sizeof (error));
        std::cerr <<  error_prefix << " Cause of:" << error << std::endl;
    }
};
#endif //AVPLAYER_FFLOGER_H
