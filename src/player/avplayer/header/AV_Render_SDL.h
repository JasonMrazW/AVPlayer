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

#define SDL_USER_EVENT_CREATE_WINDOW_DISPLAY 0x01
#define SDL_USER_EVENT_CREATE_TEXTURE 0x02
#define SDL_USER_EVENT_OPEN_AUDIO_DEVICE 0x03
#define SDL_USER_EVENT_ON_FRAME_AVAILABLE 0x04

class AV_Render_SDL {
public:
    AV_Render_SDL();
    ~AV_Render_SDL();

    bool init();

    void start();

    bool openAudioDevice(SDL_AudioFormat audio_format, uint16_t nb_samples, int freq, uint8_t channels);

    bool openVideoDevice(uint8_t *data, int width, int height, Uint32 format, int pin);

    void sendEvent(uint32_t event_type, void *data);

    static Uint32 SDL_TimerCallback(Uint32 interval, void *param);

    void updateBuffer(ThreadSafeQueue<YUVFileData> *yuv_buffer, ThreadSafeQueue<PCMItem> *pcm_buffer);
private:
    bool running = false;
    static const uint8_t TIME_INTERVAL = 40; //40ms
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
