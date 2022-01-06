//
// Created by bo on 2022/1/6.
//

#include "header/LocalFilePusher.h"

void LocalFilePusher::start(std::string dest_url) {
    AVFormatContext *input_context = nullptr;
    std::string input_url = "resources/video/sample2.flv";
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

    avformat_close_input(&input_context);
}

void LocalFilePusher::stop() {

}