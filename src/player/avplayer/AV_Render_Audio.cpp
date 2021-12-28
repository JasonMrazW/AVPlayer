//
// Created by bo on 2021/12/20.
//

#include "header/AV_Render_Audio.h"
using namespace std;

AV_Render_Audio::AV_Render_Audio() {
    master_clock = new Clock();
    SyncClock::initClock(master_clock);
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

    SDL_AudioSpec spec;

    if ((audioDeviceId = SDL_OpenAudioDevice(nullptr,0,audioSpec,&spec, SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2) {
        cerr << "SDL_Init Audio Device Failed: " << SDL_GetError() << endl;
        return false;
    }

    hw_bytes_per_sec = spec.size;
//    cout << "hw size:" << hw_bytes_per_sec << endl;
    SDL_PauseAudioDevice(audioDeviceId, 0);
}

//音频设备的回调
void AV_Render_Audio::fillDataCallBack(void *userdata, uint8_t * stream, int len) {
    SDL_memset(stream, 0, len);
    AV_Render_Audio *render = static_cast<AV_Render_Audio *>(userdata);

    //获取的是一个相对时间，不是绝对时间
    render->audio_callback_time = av_gettime_relative();

    len = len > render->audio_length.value ? render->audio_length.value : len;

    //混合播放
    SDL_MixAudioFormat(stream, render->audio_pos, render->audioFormat, len, SDL_MIX_MAXVOLUME);
    render->audio_pos +=len;
    render->audio_length.value -=len;

    double last_clock_time = SyncClock::getClockTime(render->master_clock);
    if (!isnan(render->current_audio_clock)) {
        SyncClock::setClockAtTime(render->master_clock,
                       render->current_audio_clock -
                       (double)(2 * render->hw_bytes_per_sec)
                       / render->bytes_per_sec,
                       render->audio_callback_time/1000000.0);
        //todo 补充同步外部时钟，暂不做实现
        clog << "audio clock distance:" << SyncClock::getClockTime(render->master_clock)-last_clock_time << endl;
    }

    //取下个item
    if(render->audio_length.value == 0) {
        PCMItem item;
        render->pcm_queue->dequeue(item);
//        std::cout << "pcm buffer size:" << render->pcm_queue->current_size << std::endl;
        render->audio_pos = item.data;
        render->audio_length.value = item.data_length;
        render->current_pts.store(item.pts);
        //更新当前item的audio_clock理论值
        render->current_audio_clock = item.audio_clock;
    }
    render->last_pts.store(render->current_pts.load());
}

void AV_Render_Audio::setBuffer(ThreadSafeQueue<PCMItem> *queue) {
    pcm_queue = queue;
}
bool AV_Render_Audio::onUpdate() {
    if (audioDeviceId < 2) {
        PCMItem item;
        pcm_queue->dequeue(item);
        audio_pos = item.data;
        audio_length.value = item.data_length;
        current_audio_clock = item.audio_clock;
        bytes_per_sec = item.byte_per_buffer;
        openAudioDevice(item.sdl_audio_format, item.nb_samples, item.freq, item.channels);
    }
    return true;
}
bool AV_Render_Audio::onRender() {
    return false;
}

Clock *AV_Render_Audio::getMasterClock() {
    return master_clock;
}

