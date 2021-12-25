//
// Created by bo on 2021/12/25.
//

#ifndef AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
#define AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
class IClockCallBack {
public:
    IClockCallBack() {

    }
    ~IClockCallBack() {

    }
    virtual int32_t getMainClock() = 0;
};
#endif //AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
