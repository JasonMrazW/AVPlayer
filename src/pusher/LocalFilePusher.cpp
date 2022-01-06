//
// Created by bo on 2022/1/6.
//

#include "header/LocalFilePusher.h"

void LocalFilePusher::start(std::string dest_url) {
    AVFormatContext *input_context = nullptr;
    std::string input_url = "resources/video/sample.flv";
    int ret = avformat_open_input(&input_context, input_url.c_str(), nullptr, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("avformat open input failed.",ret);
        return;
    }

    ret = avformat_find_stream_info(input_context, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("avformat find stream info failed.", ret);
        return;
    }

    av_dump_format(input_context, 0, nullptr, 0);

    readAVPacket(input_context, dest_url);

    avformat_close_input(&input_context);
}

void LocalFilePusher::stop() {

}

void LocalFilePusher::readAVPacket(AVFormatContext *inputContext, std::string dest_url) {
    int video_stream_index = -1;
    int audio_stream_index = -1;
    for (int i = 0; i < inputContext->nb_streams; ++i) {
        AVStream *input_stream = inputContext->streams[i];
        switch (input_stream->codecpar->codec_type) {
            case AVMEDIA_TYPE_AUDIO:
                audio_stream_index = i;
                break;
            case AVMEDIA_TYPE_VIDEO:
                video_stream_index = i;
                break;
            default:
                break;
        }
    }

    AVFormatContext *output_format = initOutputFormat(inputContext, dest_url);
    if (output_format == nullptr) {
        FFLoger::printErrorMessage("init output format failed.", -1);
        return;
    }

    //open output io
    int ret = -1;
    if (!(output_format->flags & AVFMT_NOFILE)) { //非文件类输出
        ret = avio_open(&output_format->pb, dest_url.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            FFLoger::printErrorMessage("open avio failed.", ret);
            return;
        }
    }

    //write header
    ret = avformat_write_header(output_format, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("write header failed.", ret);
        return;
    }

    AVPacket *temp_packet = av_packet_alloc();
    while (av_read_frame(inputContext, temp_packet) >= 0) {


        if (temp_packet->stream_index == video_stream_index) {

        } else if (temp_packet->stream_index == audio_stream_index) {

        }
    }
}

AVFormatContext * LocalFilePusher::initOutputFormat(const AVFormatContext *inputContext, const std::string &dest_url) const {
    AVFormatContext *outputContext = nullptr;
    avformat_alloc_output_context2(&outputContext, nullptr, "flv", dest_url.c_str());

    for (int i = 0; i < inputContext->nb_streams; ++i) {
        AVStream *input_stream = inputContext->streams[i];
        AVCodec *input_codec = avcodec_find_decoder(input_stream->codecpar->codec_id);
        if (input_codec == nullptr) {
            FFLoger::printErrorMessage("find codec failed.", errno);
            break;
        }
        AVStream *out_stream = avformat_new_stream(outputContext, input_codec);
        if (out_stream == nullptr) {
            FFLoger::printErrorMessage("init output stream failed.", errno);
            break;
        }

        int ret = avcodec_parameters_copy(out_stream->codecpar, input_stream->codecpar);
        if (ret < 0) {
            FFLoger::printErrorMessage("copy parameters failed.", ret);
            break;
        }
    }
    av_dump_format(outputContext, 0, dest_url.c_str(), 1);

    return outputContext;
}
