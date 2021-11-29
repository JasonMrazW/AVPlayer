//
// Created by bo on 2021/11/29.
//

#include "SDLAudioPlayer.h"

SDLAudioPlayer::SDLAudioPlayer() : running(false){

}

SDLAudioPlayer::~SDLAudioPlayer() {

}

int SDLAudioPlayer::onInit(char *audioPath) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_STOP;
    }

    //init audio info
    audioSpec = SDL_AudioSpec();
    audioSpec.samples = 44100;
    audioSpec.freq = 1024;
    audioSpec.format = AUDIO_S16LSB;
    audioSpec.channels = 2;
    audioSpec.silence = 0;
    audioSpec.callback = fillDataCallBack;

    if (SDL_OpenAudio(&audioSpec, NULL) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_STOP;
    }

    return APP_START;
}

void SDLAudioPlayer::onClean() {

}

void SDLAudioPlayer::play(char *audioPath) {
    int state = onInit(audioPath);
    if (state != APP_START) {
        return;
    }
}

void SDLAudioPlayer::fillDataCallBack(void *userdata, Uint8 * stream, int len) {

}
