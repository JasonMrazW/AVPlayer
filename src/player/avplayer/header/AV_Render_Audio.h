//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_AUDIO_H
#define AVPLAYER_AV_RENDER_AUDIO_H
#include "AV_Render_Interface.h"
#include "SDL.h"

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
    bool onRender() override{
      return false;
    };
    bool onStop() override;
    bool onDestroy() override;

    bool openDevice(void * params) override{
        AudioRenderParameters *parameters = static_cast<AudioRenderParameters *>(params);
        bool ret = openAudioDevice(parameters->audio_format, parameters->nb_samples, parameters->freq, parameters->channels);
        delete parameters;
        return ret;
    }

    //audio device callback
    static void fillDataCallBack(void *userdata, uint8_t *stream, int len);
private:
    SDL_AudioSpec *audioSpec;
    SDL_AudioDeviceID audioDeviceId;
    uint8_t audioFormat;
    SDL_atomic_t  audio_length; // audio file length
    uint8_t *audio_pos; //current position

    bool openAudioDevice(SDL_AudioFormat audio_format, uint16_t nb_samples, int freq, uint8_t channels);
};


#endif //AVPLAYER_AV_RENDER_AUDIO_H
