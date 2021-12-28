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

    AVRational tb = (AVRational){1, av_frame->sample_rate};
    if (av_frame->pts != AV_NOPTS_VALUE) {
        //转换为跟采样率有关的时间
        av_frame->pts = av_rescale_q(av_frame->pts, av_stream->time_base, tb);
    } else if (next_pts != AV_NOPTS_VALUE){ //当前音频帧没有pts，就直接使用之前计算好的pts
        av_frame->pts = av_rescale_q(next_pts, next_pts_tb, tb);
    }

    //计算下一个音频帧的pts，以防止下一帧没有pts出现
    if (av_frame->pts != AV_NOPTS_VALUE) {
        next_pts = av_frame->pts + av_frame->nb_samples;
        next_pts_tb = tb;
    }



    item->data_length = buffer_size;
    item->sdl_audio_format = ConvertUtil::AVSampleFormatToSDLAudioFormat(out_format);
    item->byte_per_buffer = av_samples_get_buffer_size(NULL, av_frame->channels,
                                                       av_frame->sample_rate,
                                                       out_format, 1);
    item->freq = av_frame->sample_rate;
    item->nb_samples = av_frame->nb_samples;
    item->channels = av_frame->channels;
    item->pts = av_frame->pts * av_q2d(tb); //换算成秒为单位
    //下一帧的理论时钟
    item->next_audio_clock = item->pts + double(av_frame->nb_samples / av_frame->sample_rate);
    item->time_base = packet_time_base;
}

void AVDecoderAudio::parseAVFrame(AVFrame *av_frame) {
    PCMItem pcm_item;
    getPCMData(av_frame, &pcm_item);
//    cout << "audio pts:" << pcm_item.pts << endl;
    //enqueue
    pcm_queue->enqueue(pcm_item);
}
