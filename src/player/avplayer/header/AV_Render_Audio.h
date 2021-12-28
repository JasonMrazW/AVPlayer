//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_AUDIO_H
#define AVPLAYER_AV_RENDER_AUDIO_H
#include "AV_Render_Interface.h"
#include "SDL.h"
#include "../../../thread/ThreadSafeQueue.h"
#include "AV_Render_RawItem.h"
#include "AV_Sync_Clock.h"


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
    bool onUpdate() override;
    bool onRender() override;
    bool onStop() override;
    bool onDestroy() override;

    //audio device callback
    static void fillDataCallBack(void *userdata, uint8_t *stream, int len);

    void setBuffer(ThreadSafeQueue<PCMItem> *queue);

    Clock *getMasterClock();

private:
    SDL_AudioSpec *audioSpec;
    SDL_AudioDeviceID audioDeviceId = 0;
    uint8_t audioFormat;
    SDL_atomic_t  audio_length; // audio file length
    uint8_t *audio_pos; //current position
    double current_audio_clock; //当前item对应的audio_clock
    double last_audio_clock; //上一个item对应的audio_clock
    Clock *master_clock;

    uint32_t hw_bytes_per_sec; //音频设备硬件缓冲区大小
    uint32_t bytes_per_sec; //每一帧音频大小
    double audio_callback_time; //音频设备回调时间


    ThreadSafeQueue<PCMItem> *pcm_queue = nullptr;
    bool openAudioDevice(SDL_AudioFormat audio_format, uint16_t nb_samples, int freq, uint8_t channels);
};


#endif //AVPLAYER_AV_RENDER_AUDIO_H
