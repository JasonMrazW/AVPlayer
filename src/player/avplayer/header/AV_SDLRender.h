//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_SDLRENDER_H
#define AVPLAYER_AV_SDLRENDER_H
#include <iostream>

#include "SDL.h"
#include "SDL_events.h"

#define SDL_USER_EVENT_CREATE_WINDOW_DISPLAY SDL_USEREVENT + 1
#define SDL_USER_EVENT_CREATE_TEXTURE SDL_USEREVENT + 2
#define SDL_USER_EVENT_OPEN_AUDIO_DEVICE SDL_USEREVENT + 3
#define SDL_USER_EVENT_ON_FRAME_AVAILABLE SDL_USEREVENT + 4

class IAVRender {
public:
    virtual bool init() = 0;
    virtual bool openDevice() = 0;
    virtual bool onUpdate() = 0;
    virtual bool onRender() = 0;
    virtual bool onStop() = 0;
    virtual bool onDestroy() = 0;
};

class AV_SDLRender {
public:
    AV_SDLRender();
    ~AV_SDLRender();

    void start();

    bool openAudioDevice();

    bool openVideoDevice();

    void sendEvent(uint32_t event_type, void *data);

    static Uint32 SDL_TimerCallback(Uint32 interval, void *param);
private:
    bool running = false;
    static const uint8_t TIME_INTERVAL = 40; //40ms
    SDL_TimerID timerId;

    IAVRender *video_render;
    IAVRender *audio_render;

    //初始化SDL
    bool onInit();

    bool onEvent(SDL_Event *sdlEvent);

    bool onRender();

    bool onStop();

    bool onDestroy();
};

#endif //AVPLAYER_AV_SDLRENDER_H
