//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_DEMUXER_H
#define AVPLAYER_AV_DEMUXER_H
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};
#include <iostream>

class AVDemuxer {
public:
    AVDemuxer() {};
    ~AVDemuxer() {

    };

    void start(const char * url);

private:
    struct AVState {
        AVFormatContext *format_context = nullptr;

        AVStream *video_stream = nullptr;
        AVStream *audio_stream = nullptr;
        uint8_t video_stream_index = -1;
        uint8_t audio_stream_index = -1;

        AVCodec *video_codec = nullptr;
        AVCodecContext *video_codecContext = nullptr;
    };

    AVState *current_state;

    bool initCodec(AVStream *video_stream, AVCodec **out_codec, AVCodecContext **out_codecContext);

    void close(AVFormatContext *formatContext);
};


#endif //AVPLAYER_AV_DEMUXER_H
