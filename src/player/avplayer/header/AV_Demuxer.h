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
#include "../../../thread/ThreadSafeQueue.h"
#include "AV_Decoder.h"
#include "AV_Decoder_VIdeo.h"

class AVDemuxer {
public:
    AVDemuxer() {

    };
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

        AVCodec *audio_codec = nullptr;
        AVCodecContext *audio_codecContext = nullptr;
    };

    ThreadSafeQueue<AVPacket> *video_packet_queue = nullptr;
    ThreadSafeQueue<AVPacket> *audio_packet_queue = nullptr;

    std::thread video_decode_thread;

    void close(AVFormatContext *formatContext);

    void readAVPackets(AVFormatContext *formatContext, AVState *state);

    static void loadVideoDecoderThreadCallback(AVDecoderVideo *video_decoder);
};


#endif //AVPLAYER_AV_DEMUXER_H
