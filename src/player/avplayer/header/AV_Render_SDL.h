//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_SDL_H
#define AVPLAYER_AV_RENDER_SDL_H
#include <iostream>

#include "SDL.h"
#include "SDL_events.h"
#include "AV_Render_Audio.h"
#include "AV_Render_Interface.h"
#include "AV_Render_Video.h"

#define SDL_USER_EVENT_ON_FRAME_AVAILABLE 0x04

class AV_Render_SDL {
public:
    AV_Render_SDL();
    ~AV_Render_SDL();

    bool init();

    void start();

    void sendEvent(uint32_t event_type, void *data);

    static Uint32 SDL_TimerCallback(Uint32 interval, void *param);

    void setBuffer(ThreadSafeQueue<YUVItem> *yuv_buffer, ThreadSafeQueue<PCMItem> *pcm_buffer);
private:
    bool running = false;
    static const uint8_t TIME_INTERVAL = 30; //30ms
    SDL_TimerID timerId;

    AV_Render_Video *video_render;
    AV_Render_Audio *audio_render;

    //初始化SDL

    bool onEvent(SDL_Event *sdlEvent);

    bool onRender();

    bool onStop();

    bool onDestroy();

};

#endif //AVPLAYER_AV_RENDER_SDL_H
