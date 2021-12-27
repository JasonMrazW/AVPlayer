//
// Created by bo on 2021/12/25.
//

#ifndef AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
#define AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
typedef struct Clock {
    double pts;           /* clock base */
    double pts_drift;     /* clock base minus time at which we updated the clock */
    double last_updated;
    double speed;
    int serial;           /* clock is based on a packet with this serial */
    int paused;
    int *queue_serial;    /* pointer to the current packet queue serial, used for obsolete clock detection */
} Clock;

class IClockCallBack {
public:
    IClockCallBack() {

    }
    ~IClockCallBack() {

    }
    virtual Clock *getMainClock() = 0;
};
#endif //AVPLAYER_AV_RENDER_ICLOCKCALLBACK_H
