//
// Created by bo on 2021/11/29.
//

#include "SDLAudioStreamPlayer.h"
SDL_atomic_t SDLAudioStreamPlayer::audio_length;
uint8_t * SDLAudioStreamPlayer::audio_pos;
uint8_t SDLAudioStreamPlayer::audioFormat;
SDL_mutex *SDLAudioStreamPlayer::mutex;

SDLAudioStreamPlayer::SDLAudioStreamPlayer() : running(false){

}

SDLAudioStreamPlayer::~SDLAudioStreamPlayer() {

}

void SDLAudioStreamPlayer::onClean() {
    SDL_Quit();
}

void SDLAudioStreamPlayer::onEvent(SDL_Event *event) {
    switch (event->type) {
        case SDL_USEREVENT: {
            break;
        }
        case SDL_QUIT:
            running = false;
            break;
    }
}

void SDLAudioStreamPlayer::startAudioPlayer() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return;
    }

    running = true;
    audio_callback = fillDataCallBack;

    mutex = SDL_CreateMutex();

    SDL_Event sdlEvent;
    while(running) {
        while(SDL_PollEvent(&sdlEvent)) {
            onEvent(&sdlEvent);
        }
    }

    free(audio_chunk);
    onClean();
}


int SDLAudioStreamPlayer::openAudioDevice(SDL_AudioFormat audio_format, uint16_t nb_samples, int freq, uint8_t channels) {
    if (audioDeviceId != 0) {
        SDL_CloseAudio();
    }
    //init audio info
    audioFormat = audio_format;
    audioSpec = SDL_AudioSpec();
    audioSpec.samples = nb_samples;
    audioSpec.freq = freq;
    audioSpec.format = audio_format;
    audioSpec.channels = channels;
    audioSpec.silence = 0;
    audioSpec.callback = audio_callback;

    if ((audioDeviceId = SDL_OpenAudioDevice(nullptr,0,&audioSpec,nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_STOP;
    }
    SDL_PauseAudioDevice(audioDeviceId, 0);
}

void SDLAudioStreamPlayer::updateAudioData(uint8_t* audio_data, uint32_t l) {
    //等前面的数据播完再解码后面的数据
    while (audio_length.value >0)
        std::this_thread::sleep_for(std::chrono::milliseconds (1));
    SDL_LockMutex(mutex);
    audio_pos = audio_data;
    audio_length.value = l;
    SDL_UnlockMutex(mutex);
}

void SDLAudioStreamPlayer::fillDataCallBack(void *userdata, uint8_t * stream, int len) {
    SDL_memset(stream, 0, len);

    len = len > audio_length.value ? audio_length.value : len;
    std::cout << "playing audio data length:" << len << std::endl;

    //混合播放
    SDL_LockMutex(mutex);
    SDL_MixAudioFormat(stream, audio_pos, audioFormat, len, SDL_MIX_MAXVOLUME);
    audio_pos +=len;
    audio_length.value -=len;
    SDL_UnlockMutex(mutex);
}

