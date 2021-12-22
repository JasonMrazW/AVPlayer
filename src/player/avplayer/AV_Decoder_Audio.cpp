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

    //init swr buffer
    //一个采样点所占的字节数
    cout << "audio sample fmt:" << codec_context->sample_fmt << endl;
    int buffer_size = av_samples_get_buffer_size(nullptr, codec_context->channels, 1024,
                                             out_sample_format, 1);
    uint8_t * output_buffer = static_cast<uint8_t *>(av_malloc(buffer_size));
    memset(output_buffer, 0, buffer_size);
}