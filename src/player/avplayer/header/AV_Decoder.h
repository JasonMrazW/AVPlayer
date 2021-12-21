//
// Created by bo on 2021/12/21.
//

#ifndef AVPLAYER_AV_DECODER_H
#define AVPLAYER_AV_DECODER_H
#include "../../../thread/ThreadSafeQueue.h"
#include <thread>
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
};

class I_AVDecoder {
public:
    I_AVDecoder(AVStream *stream, ThreadSafeQueue<AVPacket> *safeQueue):av_stream(stream), av_packet_queue(safeQueue) {
        isCodecInited = initCodec(stream,&codec, &codec_context);
    }
    ~I_AVDecoder() {
        av_stream = nullptr;
        av_packet_queue = nullptr;
    }

    bool initCodec(AVStream *video_stream, AVCodec **out_codec, AVCodecContext **out_codecContext)  {
        AVCodec *temp_codec= avcodec_find_decoder(video_stream->codecpar->codec_id);
        if (temp_codec == nullptr) {
            cerr << "start codec failed." << strerror(errno) << endl;
            return false;
        }
        *out_codec = temp_codec;


        AVCodecContext *temp_context = avcodec_alloc_context3(temp_codec);
        *out_codecContext = temp_context;
        avcodec_parameters_to_context(temp_context, video_stream->codecpar);
        int ret = avcodec_open2(temp_context, temp_codec, nullptr);
        if (ret < 0) {
            cerr << "open codec failed." << strerror(errno) << endl;
            return false;
        }

        return true;
    }

    virtual void start() = 0;

protected:
    AVStream *av_stream;
    AVCodec *codec;
    AVCodecContext *codec_context;
    ThreadSafeQueue<AVPacket> *av_packet_queue;

    atomic_bool running;

    virtual void stop() = 0;

    atomic_bool isCodecInited;
};
#endif //AVPLAYER_AV_DECODER_H
