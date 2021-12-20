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
void AV_Render_SDL::start() {
    if (!onInit()) {
        return;
    }
    running = true;

    SDL_Event event;
    int ret = 0;
    while (running) {
        while (SDL_PollEvent(&event)) {
            onEvent(&event);
        }
    }

    onStop();
}

bool AV_Render_SDL::openAudioDevice(SDL_AudioFormat audio_format, uint16_t nb_samples, int freq, uint8_t channels) {
    AudioRenderParameters parameters;
    parameters.audio_format = audio_format;
    parameters.nb_samples = nb_samples;
    parameters.channels = channels;

    sendEvent(SDL_USER_EVENT_OPEN_AUDIO_DEVICE, &parameters);
    return true;
}

bool AV_Render_SDL::openVideoDevice(uint8_t *data, int width, int height, Uint32 format, int pin) {
    VideoRenderParameters parameters;
    parameters.data = data;
    parameters.width = width;
    parameters.height = height;
    parameters.format = format;
    parameters.pin = pin;

    sendEvent(SDL_USER_EVENT_CREATE_TEXTURE, &parameters);
    return true;
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
bool AV_Render_SDL::onInit() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "init sdl failed." << strerror(errno) << endl;
        return false;
    }
    //初始化定时器，用于刷新视频帧
    timerId = SDL_AddTimer(TIME_INTERVAL, &SDL_TimerCallback, this);

    //初始化Video和Audio
    audio_render = new AV_Render_Audio();
    video_render = new AV_Render_Video();
    return true;
}

bool AV_Render_SDL::onEvent(SDL_Event *sdlEvent) {

    if (sdlEvent->type >= SDL_USEREVENT) {
        SDL_UserEvent userEvent = sdlEvent->user;
        switch (userEvent.code) {
            case SDL_USER_EVENT_CREATE_WINDOW_DISPLAY:
                //初始化SDL渲染相关组件
                video_render->init();
                break;
            case SDL_USER_EVENT_CREATE_TEXTURE:
                //根据要播放的视频信息，创建Texture
                video_render->openDevice(userEvent.data1);
                break;
            case SDL_USER_EVENT_OPEN_AUDIO_DEVICE:
                //根据要播放的音频信息，打开音频播放设备
                audio_render->openDevice(userEvent.data1);
                break;
            case SDL_USER_EVENT_ON_FRAME_AVAILABLE:
                cout << "on frame available." << endl;
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
    return video_render->onRender();
}
