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
