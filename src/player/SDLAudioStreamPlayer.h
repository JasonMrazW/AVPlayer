//
// Created by bo on 2021/12/17.
//

#ifndef AVPLAYER_SDLAUDIOSTREAMPLAYER_H
#define AVPLAYER_SDLAUDIOSTREAMPLAYER_H
#include <SDL.h>
#include <iostream>
#include <fstream>
#include <SDL_events.h>
#include <cmath>
#include <thread>

class SDLAudioStreamPlayer {
public:
    SDLAudioStreamPlayer();
    ~SDLAudioStreamPlayer();

    enum STATE {
        APP_STOP = 0,
        APP_START = 1
    };

    static void fillDataCallBack(void *userdata, uint8_t *stream, int len);

    void startAudioPlayer();

    int openAudioDevice(uint16_t audio_format, uint16_t ns_samples, int freq, uint8_t channels);

    void updateAudioData(uint8_t* audio_data, uint32_t audio_length);
private:
    bool running = false;
    uint8_t *audio_chunk;
    static SDL_atomic_t  audio_length; // audio file length
    static uint8_t *audio_pos; //current position
    static uint8_t audioFormat;
    SDL_AudioSpec audioSpec;
    SDL_AudioDeviceID audioDeviceId;
    int8_t pcm_buffer;
    SDL_AudioCallback audio_callback;
    static SDL_mutex *mutex;

    const int pcm_buffer_size = 4096*2;

    void onClean();

    void onEvent(SDL_Event *event);
};



#endif //AVPLAYER_SDLAUDIOSTREAMPLAYER_H
