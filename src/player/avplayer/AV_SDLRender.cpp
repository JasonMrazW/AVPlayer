//
// Created by bo on 2021/12/20.
//

#include "header/AV_SDLRender.h"

using namespace std;

AV_SDLRender::AV_SDLRender() {

}

AV_SDLRender::~AV_SDLRender() {
    onDestroy();
}

//******************public method******************
void AV_SDLRender::start() {
    if (!onInit()) {
        return;
    }
    running = true;

    SDL_Event event;
    while(running) {
        if (SDL_PollEvent(&event)) {
            onEvent(&event);
        }
    }

    onStop();
}

bool AV_SDLRender::openAudioDevice(SDL_AudioFormat audio_format, uint16_t nb_samples, int freq, uint8_t channels) {
    AudioRenderParameters parameters;
    parameters.audio_format = audio_format;
    parameters.nb_samples = nb_samples;
    parameters.channels = channels;

    sendEvent(SDL_USER_EVENT_OPEN_AUDIO_DEVICE, &parameters);
    return  true;
}

bool AV_SDLRender::openVideoDevice(uint8_t *data, int width, int height, Uint32 format, int pin) {
    VideoRenderParameters parameters;
    parameters.data = data;
    parameters.width = width;
    parameters.height = height;
    parameters.format = format;
    parameters.pin = pin;

    sendEvent(SDL_USER_EVENT_CREATE_TEXTURE, &parameters);
    return  true;
}

Uint32 AV_SDLRender::SDL_TimerCallback(Uint32 interval, void *param) {
    AV_SDLRender *sdl_render = static_cast<AV_SDLRender *>(param);
    sdl_render->sendEvent(SDL_USER_EVENT_ON_FRAME_AVAILABLE, nullptr);
    return (interval);
}

void AV_SDLRender::sendEvent(uint32_t event_type, void *data) {
    SDL_Event event;
    event.type = event_type;

    SDL_UserEvent userEvent;
    userEvent.data1 = data;
    event.user = userEvent;
    SDL_PushEvent(&event);
}
//******************private method******************
bool AV_SDLRender::onInit() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "init sdl failed." << strerror(errno) << endl;
        return false;
    }
    //初始化定时器，用于刷新视频帧
    timerId = SDL_AddTimer(TIME_INTERVAL, &SDL_TimerCallback, this);

    //初始化Video和Audio
    audio_render = new AV_AudioRender();
    video_render = new AV_VideoRender();
    return true;
}

bool AV_SDLRender::onEvent(SDL_Event *sdlEvent) {
    clog << "receive sdl event:" << sdlEvent->type << endl;
    switch (sdlEvent->type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent->key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            break;
        case SDL_USER_EVENT_CREATE_WINDOW_DISPLAY:
            //初始化SDL渲染相关组件
            video_render->init();
            break;
        case SDL_USER_EVENT_CREATE_TEXTURE:
            //根据要播放的视频信息，创建Texture
            video_render->openDevice(sdlEvent->user.data1);
            break;
        case SDL_USER_EVENT_OPEN_AUDIO_DEVICE:
            //根据要播放的音频信息，打开音频播放设备
            audio_render->openDevice(sdlEvent->user.data1);
            break;
        case SDL_USER_EVENT_ON_FRAME_AVAILABLE:
            onRender();
            break;
        default:
            break;
    }
    return false;
}

bool AV_SDLRender::onStop() {
    audio_render->onStop();
    video_render->onStop();

    SDL_RemoveTimer(timerId);
    SDL_Quit();

    return true;
}

bool AV_SDLRender::onDestroy() {
    return true;
}

//控制视频的帧率
bool AV_SDLRender::onRender() {
    return video_render->onRender();
}
