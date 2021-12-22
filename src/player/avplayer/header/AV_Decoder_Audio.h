//
// Created by bo on 2021/12/22.
//

#ifndef AVPLAYER_AV_DECODER_AUDIO_H
#define AVPLAYER_AV_DECODER_AUDIO_H
#include "AV_Decoder.h"

class AVDecoderAudio : public I_AVDecoder{
public:
    AVDecoderAudio(AVStream *avStream, ThreadSafeQueue<AVPacket> *safeQueue);

    ~AVDecoderAudio();

    void start();
    void stop();
private:
    SwrContext *swr_context;
    static const AVSampleFormat out_sample_format = AV_SAMPLE_FMT_S16; //默认音频播放格式
};


#endif //AVPLAYER_AV_DECODER_AUDIO_H
