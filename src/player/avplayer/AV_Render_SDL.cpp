//
// Created by bo on 2021/12/20.
//

#include "header/AV_Render_SDL.h"

using namespace std;

AV_Render_SDL::AV_Render_SDL() {
}

AV_Render_SDL::~AV_Render_SDL() {
    onDestroy();
}

//******************public method******************
bool AV_Render_SDL::init(uint8_t video_fps) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "init sdl failed." << strerror(errno) << endl;
        return false;
    }
    //初始化定时器，用于刷新视频帧
//    timerId = SDL_AddTimer(video_fps, &SDL_TimerCallback, this);
    refresh_rate = 1000/video_fps; //单位ms

    //初始化Video和Audio
    audio_render = new AV_Render_Audio();
    video_render = new AV_Render_Video(this);
    return true;
}

void AV_Render_SDL::start() {
    running = true;
    SDL_Event event;

    //初始化SDL渲染相关组件
    video_render->init();

    while (running) {
        sendEvent(SDL_USER_EVENT_ON_FRAME_AVAILABLE, nullptr);
        while (SDL_PollEvent(&event)) {
            onEvent(&event);
        }
    }

    onStop();
    onDestroy();
}

void AV_Render_SDL::setBuffer(ThreadSafeQueue<YUVItem> *yuv_buffer, ThreadSafeQueue<PCMItem> *pcm_buffer) {
    video_render->setBuffer(yuv_buffer);
    audio_render->setBuffer(pcm_buffer);
}

void AV_Render_SDL::sendEvent(uint32_t event_type, void *data) {

    Uint32 myEventType = SDL_RegisterEvents(1);
    if (myEventType != ((Uint32) -1)) {
        SDL_Event event;
        SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
        event.type = myEventType;
        event.user.code = event_type;
        event.user.data1 = data;
        event.user.data2 = 0;
        SDL_PushEvent(&event);
    }
}

//******************private method******************


bool AV_Render_SDL::onEvent(SDL_Event *sdlEvent) {
    if (sdlEvent->type >= SDL_USEREVENT) {
        SDL_UserEvent userEvent = sdlEvent->user;
        switch (userEvent.code) {
            case SDL_USER_EVENT_ON_FRAME_AVAILABLE:{
                double t = onUpdate();
                onRender(t);
                break;
            }
            default:
                break;
        }
    } else {
        switch (sdlEvent->type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent->key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                break;
            default:
                break;
        }
    }

    return false;
}

bool AV_Render_SDL::onStop() {
    audio_render->onStop();
    video_render->onStop();

    SDL_RemoveTimer(timerId);
    SDL_Quit();

    return true;
}

bool AV_Render_SDL::onDestroy() {
    video_render->onDestroy();
    audio_render->onDestroy();
    return true;
}

double AV_Render_SDL::onUpdate() {
    double remaining_time = refresh_rate;
    video_render->onUpdate(&remaining_time);
    audio_render->onUpdate(&remaining_time);
    return remaining_time;
}

//控制视频的帧率
bool AV_Render_SDL::onRender(double remaining_time) {
    video_render->onRender();
    audio_render->onRender();
    //wait
    std::this_thread::sleep_for(std::chrono::microseconds((int)remaining_time));
    sendEvent(SDL_USER_EVENT_ON_FRAME_AVAILABLE, nullptr);
    return true;
}

Clock *AV_Render_SDL::getMainClock() {
    return audio_render->getMasterClock();
}
