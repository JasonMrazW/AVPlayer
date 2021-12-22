//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_INTERFACE_H
#define AVPLAYER_AV_RENDER_INTERFACE_H
#include <iostream>
#include "SDL.h"
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
};
#endif //AVPLAYER_AV_RENDER_INTERFACE_H
