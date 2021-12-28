//
// Created by bo on 2021/12/25.
//

#ifndef AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
#define AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
#include "AV_Sync_Clock.h"
class IClockCallBack {
public:
    IClockCallBack() {

    }
    ~IClockCallBack() {

    }
    virtual Clock *getMainClock() = 0;
};
#endif //AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
