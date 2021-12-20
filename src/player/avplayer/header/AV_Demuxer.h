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
    ~AVDemuxer() {};

    void init(const char * url);

private:
    bool initCodec(AVStream *video_stream, AVCodec **out_codec, AVCodecContext **out_codecContext);

    void close();
};


#endif //AVPLAYER_AV_DEMUXER_H
