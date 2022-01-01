//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_INTERFACE_H
#define AVPLAYER_AV_RENDER_INTERFACE_H
#include <iostream>
#include "SDL.h"
#include "AV_Render_IClockCallback.h"

class IAVRender {
public:
    IAVRender() {
        last_pts.store(0);
        current_pts.store(0);
    }
    ~IAVRender() {

    }
    virtual bool init() = 0;

    virtual bool onUpdate(double *remaining_time) = 0;
    virtual bool onRender() = 0;
    virtual bool onStop() = 0;
    virtual bool onDestroy() = 0;

    int32_t getLastPts() {
        return last_pts;
    }
protected:
    //上一个已渲染帧对应的pts
    std::atomic<double> last_pts;
    //将要渲染帧对应的pts
    std::atomic<double> current_pts;

    double time_base;
};
#endif //AVPLAYER_AV_RENDER_INTERFACE_H
