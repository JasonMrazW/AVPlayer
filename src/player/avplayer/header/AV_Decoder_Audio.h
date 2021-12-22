//
// Created by bo on 2021/12/22.
//

#ifndef AVPLAYER_AV_DECODER_AUDIO_H
#define AVPLAYER_AV_DECODER_AUDIO_H
#include "AV_Decoder.h"
#include "AV_PCM_DataItem.h"
#include "../../../thread/ThreadSafeQueue.h"

class AVDecoderAudio : public I_AVDecoder{
public:
    AVDecoderAudio(AVStream *avStream, ThreadSafeQueue<AVPacket> *safeQueue): I_AVDecoder(avStream, safeQueue) {
        init();
    }

    ~AVDecoderAudio() {
        av_free(av_stream);
        swr_free(&swr_context);
    }

    void start();
    void stop();
private:
    SwrContext *swr_context;

    ThreadSafeQueue<PCMItem> *pcm_queue;

    static const AVSampleFormat out_sample_format = AV_SAMPLE_FMT_S16; //默认音频播放格式

    void init();

    void getPCMData(AVFrame *av_frame, PCMItem *item);
};


#endif //AVPLAYER_AV_DECODER_AUDIO_H
