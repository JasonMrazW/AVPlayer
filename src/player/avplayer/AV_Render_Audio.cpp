//
// Created by bo on 2021/12/20.
//

#include "header/AV_Render_Audio.h"
using namespace std;


AV_Render_Audio::AV_Render_Audio() {

}

AV_Render_Audio::~AV_Render_Audio() {
    onDestroy();
}

bool AV_Render_Audio::init() {
    return false;
}

bool AV_Render_Audio::onStop() {
    if (audioDeviceId != 0) {
        SDL_CloseAudioDevice(audioDeviceId);
        audioDeviceId = 0;
    }
    return true;
}

bool AV_Render_Audio::onDestroy() {
    delete audioSpec;
    delete audio_pos;
    return true;
}

bool AV_Render_Audio::openAudioDevice(SDL_AudioFormat audio_format, uint16_t nb_samples, int freq, uint8_t channels) {
    onStop();
    //init audio info
    audioFormat = audio_format;
    audioSpec = new SDL_AudioSpec();
    audioSpec->samples = nb_samples;
    audioSpec->freq = freq;
    audioSpec->format = audio_format;
    audioSpec->channels = channels;
    audioSpec->silence = 0;
    audioSpec->callback = &fillDataCallBack;
    audioSpec->userdata = this;

    if ((audioDeviceId = SDL_OpenAudioDevice(nullptr,0,audioSpec,nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2) {
        cerr << "SDL_Init Audio Device Failed: " << SDL_GetError() << endl;
        return false;
    }
    SDL_PauseAudioDevice(audioDeviceId, 0);
}

//音频设备的回调
void AV_Render_Audio::fillDataCallBack(void *userdata, uint8_t * stream, int len) {
    SDL_memset(stream, 0, len);

    AV_Render_Audio *render = static_cast<AV_Render_Audio *>(userdata);

    len = len > render->audio_length.value ? render->audio_length.value : len;
    std::cout << "playing audio data length:" << len << std::endl;

    //混合播放
    SDL_MixAudioFormat(stream, render->audio_pos, render->audioFormat, len, SDL_MIX_MAXVOLUME);
    render->audio_pos +=len;
    render->audio_length.value -=len;

    //取下个item
    if(render->audio_length.value == 0) {
        PCMItem item;
        render->pcm_queue->dequeue(item);
//        std::cout << "playing audio data length:" << render->pcm_queue->current_size << std::endl;
        render->audio_pos = item.data;
        render->audio_length.value = item.data_length;
    }
}

void AV_Render_Audio::setBuffer(ThreadSafeQueue<PCMItem> *queue) {
    pcm_queue = queue;
}

bool AV_Render_Audio::onRender() {
    if (audioDeviceId < 2) {
        PCMItem item;
        pcm_queue->dequeue(item);
        audio_pos = item.data;
        audio_length.value = item.data_length;
        openAudioDevice(item.audio_format, item.nb_samples, item.freq, item.channels);
    }
    return false;
}
