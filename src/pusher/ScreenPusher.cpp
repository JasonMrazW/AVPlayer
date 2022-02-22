//
// Created by bo on 2022/2/21.
//

#include "header/ScreenPusher.h"
using namespace std;

AVFormatContext *ScreenPusher::initInputContext() {
    AVFormatContext *input_context = avformat_alloc_context();
    avdevice_register_all();
    AVDictionary *dictionary = nullptr;
    av_dict_set(&dictionary, "list_devices", "true", 0);

    AVInputFormat *inputFormat = av_find_input_format("avfoundation");
    int ret = avformat_open_input(&input_context, "0:0", inputFormat, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("failed to open input device.", ret);
    }

    ret = avformat_find_stream_info(input_context, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("failed to open input device.", ret);
    }

    return input_context;
}

AVFormatContext *ScreenPusher::initOutputContext(AVFormatContext *inputContext, std::string destUrl) {
    AVFormatContext *outputContext = nullptr;
    avformat_alloc_output_context2(&outputContext, nullptr, "flv", destUrl.c_str());

    for (int i = 0; i < inputContext->nb_streams; ++i) {
        //初始化解码相关参数
        AVStream *stream = inputContext->streams[i];
        AVCodec *decoder = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext *decoderContext = avcodec_alloc_context3(decoder);
        avcodec_parameters_to_context(decoderContext, stream->codecpar);
        int ret = avcodec_open2(decoderContext, decoder, nullptr);
        if (ret < 0) {
            FFLoger::printErrorMessage("failed to open decoder.", ret);
            return nullptr;
        }
        decoderContext->codec_tag = 0;
        if (decoderContext->flags & AVFMT_GLOBALHEADER) {
            decoderContext->codec_tag |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        //初始化编码相关参数
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
            AVCodecContext *encoderContext = avcodec_alloc_context3(encoder);
            encoderContext->framerate = {25, 1};
            encoderContext->time_base = {1, 30};
            encoderContext->width = stream->codecpar->width;
            encoderContext->height = stream->codecpar->height;
            encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;
            encoderContext->gop_size = 30;
            encoderContext->codec_id = AV_CODEC_ID_H264;
            encoderContext->codec_type = AVMEDIA_TYPE_VIDEO;
            av_opt_set(encoderContext->priv_data, "preset", "slow", 0);

            ret = avcodec_open2(encoderContext, encoder, nullptr);
            if (ret < 0) {
                FFLoger::printErrorMessage("failed to open video encoder.", ret);
                return nullptr;
            }

            AVStream *outStream = avformat_new_stream(outputContext, encoder);
            outStream->codecpar->bit_rate = 2000*1000;
            outStream->codecpar->format = AV_PIX_FMT_YUV420P;
            outStream->codecpar->width = stream->codecpar->width;
            outStream->codecpar->height = stream->codecpar->height;
            outStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
            outStream->codecpar->codec_id = AV_CODEC_ID_H264;
        } else {
            AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
            AVCodecContext *encoderContext = avcodec_alloc_context3(encoder);
            encoderContext->sample_rate = 48000;
            encoderContext->profile = FF_PROFILE_AAC_MAIN;
            encoderContext->time_base = {1, 48000};
            encoderContext->sample_fmt = encoder->sample_fmts[0];
            encoderContext->channels = 2;
            encoderContext->channel_layout = AV_CH_LAYOUT_STEREO;
            encoderContext->codec_id = AV_CODEC_ID_AAC;
            encoderContext->codec_type = AVMEDIA_TYPE_AUDIO;

            ret = avcodec_open2(encoderContext, encoder, nullptr);
            if (ret < 0) {
                FFLoger::printErrorMessage("failed to open audio encoder.", ret);
                return nullptr;
            }

            AVStream *outStream = avformat_new_stream(outputContext, encoder);
            outStream->codecpar->bit_rate = stream->codecpar->bit_rate;
            outStream->codecpar->sample_rate = 48000;
            outStream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
            outStream->codecpar->codec_id = AV_CODEC_ID_AAC;
            outStream->codecpar->format = AV_SAMPLE_FMT_S16;
            outStream->codecpar->channels = 2;
            outStream->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
            if (ret < 0) {
                FFLoger::printErrorMessage("failed to copy codec parameters.", ret);
                return nullptr;
            }
        }
    }

    av_dump_format(outputContext, 0, destUrl.c_str(), 1);

    return outputContext;
}

void ScreenPusher::sendAVPacket(AVFormatContext *inputContext, AVFormatContext *outputContext, std::string destUrl) {
    //set frame rate,1秒30帧,单位microseconds
    uint32_t frame_interval_time = 1000*1000/30;

    //write header
    int ret = -1;
    if (!(outputContext->flags & AVFMT_NOFILE)) {
        ret = avio_open(&outputContext->pb,destUrl.c_str(), AVIO_FLAG_WRITE);
        if(ret < 0) {
            FFLoger::printErrorMessage("failed to open av io.", ret);
            return;
        }
    }

    ret = avformat_write_header(outputContext, nullptr);
    if(ret < 0) {
        FFLoger::printErrorMessage("failed to open av io.", ret);
        return;
    }
}
