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

    void start() {
        running = true;
        AVPacket *av_packet = av_packet_alloc();
        AVFrame *av_frame = av_frame_alloc();

        int ret;
        int index = 0;
        while (isCodecInited&&running) {
            //send pack to decoder
            av_packet_queue->dequeue(*av_packet);
            ret = avcodec_send_packet(codec_context,av_packet);
            if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                cerr << "video codec send packed failed. error code :" << ret << endl;
                continue;
            }
            //receive frame from decoder
            while(ret >= 0) {
                ret = avcodec_receive_frame(codec_context, av_frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    //receive frame end
                    break;
                }

                //get yuv data & add to yuv buffer
                parseAVFrame(av_frame);
                av_frame_unref(av_frame);
            }
        }
    }

    virtual void parseAVFrame(AVFrame *av_frame) = 0;
    virtual void stop() = 0;
protected:
    AVStream *av_stream;
    AVCodec *codec;
    AVCodecContext *codec_context;
    ThreadSafeQueue<AVPacket> *av_packet_queue;

    atomic_bool running;
    atomic_bool isCodecInited;
};
#endif //AVPLAYER_AV_DECODER_H
