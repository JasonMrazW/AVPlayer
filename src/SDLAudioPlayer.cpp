//
// Created by bo on 2021/11/29.
//

#include "SDLAudioPlayer.h"

Uint8 * SDLAudioPlayer::audio_track;
Uint32 SDLAudioPlayer::audio_length;
Uint8 * SDLAudioPlayer::audio_pos;
char * SDLAudioPlayer::pcm_buffer;

const Uint32 SDLAudioPlayer::SDL_EVENT_BUFFER_END = 1;

SDLAudioPlayer::SDLAudioPlayer() : running(false){

}

SDLAudioPlayer::~SDLAudioPlayer() {

}

struct CallbackObject
        {
    void onCallback(Uint8 *stream, int len)
    {
        // ....
        std::cout << "cout " << len << std::endl;
    }

    static void forwardCallback(void *userdata, Uint8 *stream, int len)
    {
        static_cast<CallbackObject*>(userdata)->onCallback(stream, len);
    }
        };

int SDLAudioPlayer::onInit(char *audioPath) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_STOP;
    }

    //init audio info
    CallbackObject callbackObject;


    audioSpec = SDL_AudioSpec();
    audioSpec.samples = 1024;
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16LSB;
    audioSpec.channels = 2;
    audioSpec.silence = 0;
    audioSpec.callback = fillDataCallBack;

    if ((audioDeviceId = SDL_OpenAudioDevice(nullptr,0,&audioSpec,&audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_STOP;
    }

    return APP_START;
}

void SDLAudioPlayer::onClean() {
    SDL_Quit();
}

void SDLAudioPlayer::onEvent(SDL_Event *event) {
    switch (event->type) {
        case SDL_USEREVENT:
            if(!audioStream.read(pcm_buffer, pcm_buffer_size)) {
                //循环播放
                audioStream.clear();
                audioStream.seekg(0, std::ios_base::beg);
                audioStream.read(pcm_buffer, pcm_buffer_size);
            }
            audio_track = (Uint8 *)pcm_buffer;
            audio_length = pcm_buffer_size;
            audio_pos = audio_track;
            std::cout << "read pcm data start" << std::endl;
            break;
        case SDL_QUIT:
            running = false;
            break;
    }
}

void SDLAudioPlayer::play(char *audioPath) {
    int state = onInit(audioPath);
    if (state != APP_START) {
        return;
    }

    SDL_Event sdlEvent;

    //open file
    audioStream.open(audioPath, std::ios::in|std::ios::binary);

    //running = true;
    pcm_buffer = new char[pcm_buffer_size];
    running = true;

    SDL_PauseAudioDevice(audioDeviceId, 0);
    SDL_CreateThread(sdl_thread_custom_event, "custom_event", nullptr);

    while(running) {
        while(SDL_PollEvent(&sdlEvent)) {
            onEvent(&sdlEvent);
        }
    }

    free(pcm_buffer);
    onClean();
}

void SDLAudioPlayer::fillDataCallBack(void *userdata, Uint8 * stream, int len) {
    SDL_memset(stream, 0, len);
    if (audio_length == 0) {
        return;
    }

    len = len > audio_length ? audio_length : len;

    //混合播放
    //SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);

    //单独播放audio_pos
    memcpy(stream, audio_pos, len);
    audio_pos += len;
    audio_length -= len;

    if (audio_length <= 0) {
        notifyGetAudioFrame();
    }
}

void SDLAudioPlayer::notifyGetAudioFrame() {
    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
}

// 线程运行函数
int SDLAudioPlayer::sdl_thread_custom_event(void *){
    // 延时 5 秒
    SDL_Delay(1000);
    // 创建自定义事件并发送到消息队列中去
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_USEREVENT;
    SDL_PushEvent(&sdlEvent);
}
