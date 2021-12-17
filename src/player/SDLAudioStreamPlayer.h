//
// Created by bo on 2021/12/17.
//

#ifndef AVPLAYER_SDLAUDIOSTREAMPLAYER_H
#define AVPLAYER_SDLAUDIOSTREAMPLAYER_H


class SDLAudioStreamPlayer {
public:
    SDLAudioStreamPlayer();
    ~SDLAudioStreamPlayer();

    enum STATE {
        APP_STOP = 0,
        APP_START = 1
    };


    static void fillDataCallBack(void *userdata, Uint8 *stream, int len);

    void play(char* audioPath);

private:
    bool running = false;

    static Uint8 *audio_chunk;
    static Uint32 audio_length; // audio file length
    static Uint8 *audio_pos; //current position
    static const Uint32 SDL_EVENT_BUFFER_END;
    static unsigned short audioFormat;
    SDL_AudioSpec audioSpec;
    SDL_AudioDeviceID audioDeviceId;

    static char * pcm_buffer;
    const int pcm_buffer_size = 4096*2;
    static const double d;
    static const double factor;
    std::ifstream audioStream;

    int onInit();

    void onClean();

    void onEvent(SDL_Event *event);


    static void notifyGetAudioFrame();

    static int sdl_thread_custom_event(void *);

    static short getShort(char high, char low);

    static void halfToVolumn(int len);

    static void playSingleChannel(int len);

    void speedupVolumn(int &len, char *&temp) const;

    unsigned char *_16BitTo8Bit() const;

    void startAudioPlayer();

    int openAudioDevice(uint16_t audio_format, uint16_t ns_samples, uint32_t freq, uint8_t channels);
};



#endif //AVPLAYER_SDLAUDIOSTREAMPLAYER_H
