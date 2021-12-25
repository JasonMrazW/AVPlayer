//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_INTERFACE_H
#define AVPLAYER_AV_RENDER_INTERFACE_H
#include <iostream>
#include "SDL.h"
typedef  int (*LastClockTime)();
class IAVRender {
public:
    IAVRender() {
    }
    ~IAVRender() {

    }
    virtual bool init() = 0;

    virtual bool onUpdate() = 0;
    virtual bool onRender() = 0;
    virtual bool onStop() = 0;
    virtual bool onDestroy() = 0;

    int32_t getLastPts() {
        return last_pts;
    }
protected:
    //上一个已渲染帧对应的pts
    std::atomic_int32_t last_pts;
    //将要渲染帧对应的pts
    std::atomic_int32_t current_pts;
};
#endif //AVPLAYER_AV_RENDER_INTERFACE_H
