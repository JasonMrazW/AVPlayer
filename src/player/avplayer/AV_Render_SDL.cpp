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
bool AV_Render_SDL::init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "init sdl failed." << strerror(errno) << endl;
        return false;
    }
    //初始化定时器，用于刷新视频帧
    timerId = SDL_AddTimer(TIME_INTERVAL, &SDL_TimerCallback, this);

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
        while (SDL_PollEvent(&event)) {
            onEvent(&event);
        }
    }

    onStop();
}

void AV_Render_SDL::setBuffer(ThreadSafeQueue<YUVItem> *yuv_buffer, ThreadSafeQueue<PCMItem> *pcm_buffer) {
    video_render->setBuffer(yuv_buffer);
    audio_render->setBuffer(pcm_buffer);
}

Uint32 AV_Render_SDL::SDL_TimerCallback(Uint32 interval, void *param) {

    AV_Render_SDL *sdl_render = static_cast<AV_Render_SDL *>(param);
    sdl_render->sendEvent(SDL_USER_EVENT_ON_FRAME_AVAILABLE, nullptr);
    return (interval);
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
            case SDL_USER_EVENT_ON_FRAME_AVAILABLE:
                onRender();
                break;
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
    return true;
}

//控制视频的帧率
bool AV_Render_SDL::onRender() {
    video_render->onRender();
    audio_render->onRender();
    return true;
}

int AV_Render_SDL::getMainClock() {
    return audio_render->getLastPts();
}
