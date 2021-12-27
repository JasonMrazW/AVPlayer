//
// Created by bo on 2021/12/22.
//

#ifndef AVPLAYER_AV_DECODER_AUDIO_H
#define AVPLAYER_AV_DECODER_AUDIO_H
#include "AV_Decoder.h"
#include "AV_Render_RawItem.h"
#include "../../../thread/ThreadSafeQueue.h"
#include "../../../util/ConvertUtil.h"

class AVDecoderAudio : public I_AVDecoder{
public:
    AVDecoderAudio(AVFormatContext *context, AVStream *avStream, ThreadSafeQueue<AVPacket> *safeQueue): I_AVDecoder(context, avStream, safeQueue) {
        init();
    }

    ~AVDecoderAudio() {
        av_free(av_stream);
        swr_free(&swr_context);
    }

    void parseAVFrame(AVFrame *av_frame) override;
    void stop() override;

    ThreadSafeQueue<PCMItem> *getPCMQueue() {
        return pcm_queue;
    }
private:
    SwrContext *swr_context;

    ThreadSafeQueue<PCMItem> *pcm_queue;
    AVSampleFormat out_format = AV_SAMPLE_FMT_S16;

    static const AVSampleFormat out_sample_format = AV_SAMPLE_FMT_S16; //默认音频播放格式

    void init();

    void getPCMData(AVFrame *av_frame, PCMItem *item);
};


#endif //AVPLAYER_AV_DECODER_AUDIO_H
