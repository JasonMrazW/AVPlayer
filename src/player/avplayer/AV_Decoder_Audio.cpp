//
// Created by bo on 2021/12/22.
//

#include "header/AV_Decoder_Audio.h"
void AVDecoderAudio::init() {
    //init swr context
    AVSampleFormat audio_sample_format = static_cast<AVSampleFormat>(av_stream->codecpar->format);
    swr_context = swr_alloc_set_opts(nullptr, AV_CH_LAYOUT_STEREO, out_sample_format,
                                     codec_context->sample_rate,
                                     codec_context->channel_layout, audio_sample_format,
                                     codec_context->sample_rate,0 ,nullptr);
    int ret = swr_init(swr_context);
    if (ret < 0) {
        cerr << "init swr context failed. error code :" << ret << endl;
    }
    pcm_queue = new ThreadSafeQueue<PCMItem>(100);
}

void AVDecoderAudio::stop() {
    running = false;
}

void AVDecoderAudio::getPCMData(AVFrame *av_frame, PCMItem *item) {
    //一帧数据的大小(表示有nb_samples的数据) = channels * nb_samples * format所占字节

    uint32_t buffer_size = av_samples_get_buffer_size(NULL, av_frame->channels, av_frame->nb_samples,
                                                 out_sample_format, 1);
    item->data = static_cast<uint8_t *>(av_malloc(buffer_size));
    memset(item->data, 0, buffer_size);;

    int samples = swr_convert(swr_context, &item->data, av_frame->nb_samples,
                              (const uint8_t **)av_frame->extended_data, av_frame->nb_samples);
    if (samples <= 0) {
        cerr << "convert audio info failed." << endl;
        return;
    }

    item->data_length = buffer_size;
    item->audio_format = ConvertUtil::AVSampleFormatToSDLAudioFormat(out_format);
    item->freq = av_frame->sample_rate;
    item->nb_samples = av_frame->nb_samples;
    item->channels = av_frame->channels;
    item->pts = av_frame->pts;
    item->time_base = packet_time_base;
}

void AVDecoderAudio::parseAVFrame(AVFrame *av_frame) {
    PCMItem pcm_item;
    getPCMData(av_frame, &pcm_item);
    //enqueue
    pcm_queue->enqueue(pcm_item);
}
