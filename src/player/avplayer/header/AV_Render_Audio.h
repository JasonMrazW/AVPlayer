//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_AUDIO_H
#define AVPLAYER_AV_RENDER_AUDIO_H
#include "AV_Render_Interface.h"
#include "SDL.h"
#include "../../../thread/ThreadSafeQueue.h"
#include "AV_Render_RawItem.h"

typedef struct Audio_Render_Parameters {
    SDL_AudioFormat audio_format;
    uint16_t nb_samples;
    int freq;
    uint8_t channels;
}AudioRenderParameters;

class AV_Render_Audio : public IAVRender{
public:
    AV_Render_Audio();
    ~AV_Render_Audio();

    bool init() override;
    bool onUpdate() override{
        return false;
    }
    bool onRender() override;
    bool onStop() override;
    bool onDestroy() override;

    //audio device callback
    static void fillDataCallBack(void *userdata, uint8_t *stream, int len);

    void setBuffer(ThreadSafeQueue<PCMItem> *queue);
private:
    SDL_AudioSpec *audioSpec;
    SDL_AudioDeviceID audioDeviceId = 0;
    uint8_t audioFormat;
    SDL_atomic_t  audio_length; // audio file length
    uint8_t *audio_pos; //current position

    ThreadSafeQueue<PCMItem> *pcm_queue = nullptr;
    bool openAudioDevice(SDL_AudioFormat audio_format, uint16_t nb_samples, int freq, uint8_t channels);
};


#endif //AVPLAYER_AV_RENDER_AUDIO_H
