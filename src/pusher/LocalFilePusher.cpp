//
// Created by bo on 2022/1/6.
//

#include "header/LocalFilePusher.h"
using namespace std;

AVFormatContext *LocalFilePusher::initInputContext() {
    AVFormatContext *input_context = nullptr;
    std::string input_url = "resources/video/sample_long.flv";
    int ret = avformat_open_input(&input_context, input_url.c_str(), nullptr, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("avformat open input failed.",ret);
        return nullptr;
    }

    ret = avformat_find_stream_info(input_context, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("avformat find stream info failed.", ret);
        return nullptr;
    }

    av_dump_format(input_context, 0, nullptr, 0);

    return input_context;
}

AVFormatContext *LocalFilePusher::initOutputContext(AVFormatContext *inputContext, std::string destUrl) {
    return Pusher::initOutputContext(inputContext, destUrl);
}

void LocalFilePusher::sendAVPacket(AVFormatContext *inputContext, AVFormatContext *outputContext, std::string destUrl) {
    Pusher::sendAVPacket(inputContext, outputContext, destUrl);
}

