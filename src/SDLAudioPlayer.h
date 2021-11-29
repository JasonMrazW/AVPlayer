//
// Created by bo on 2021/11/29.
//

#ifndef AVPLAYER_SDLAUDIOPLAYER_H
#define AVPLAYER_SDLAUDIOPLAYER_H
#include <SDL.h>

class SDLAudioPlayer {
public:
    SDLAudioPlayer();
    ~SDLAudioPlayer();

    enum STATE {
        APP_STOP = 0,
        APP_START = 1
    };

    void play(char* audioPath);

private:
    bool running = false;

    Uint8 *buffer;
    Uint32 length; // audio file length
    SDL_AudioSpec audioSpec;

    int onInit(char *audioPath);

    void onClean();

    static void fillDataCallBack(void *userdata, Uint8 *stream, int len);
};


#endif //AVPLAYER_SDLAUDIOPLAYER_H
