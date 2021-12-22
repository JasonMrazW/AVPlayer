//
// Created by bo on 2021/12/21.
//

#ifndef AVPLAYER_AV_DECODER_VIDEO_H
#define AVPLAYER_AV_DECODER_VIDEO_H
#include "AV_Decoder.h"
#include "../../../parser/header/YUVFileData.h"
#include "../../../util/ConvertUtil.h"

class AVDecoderVideo : public I_AVDecoder{
public:
    AVDecoderVideo(AVStream *stream, ThreadSafeQueue<AVPacket> *safeQueue): I_AVDecoder(stream, safeQueue) {
        yuv_queue = new ThreadSafeQueue<YUVFileData>(25);
        //根据图像格式，计算出一帧图像的大小
        video_format = static_cast<AVPixelFormat>(stream->codecpar->format);
        buffer_size = av_image_get_buffer_size(video_format, codec_context->width, codec_context->height, 1);
        out_buffer = static_cast<uint8_t *>(av_malloc(buffer_size));

        sws_context = sws_getContext(codec_context->width, codec_context->height,video_format,
                                                 codec_context->width, codec_context->height,video_format,
                                                 SWS_BICUBIC, nullptr,nullptr,nullptr);
    };

    ~AVDecoderVideo() {
        av_free(out_buffer);
        sws_freeContext(sws_context);
    };

    void start() override;
    void stop() override;
    ThreadSafeQueue<YUVFileData> *getYUVQueue() {
        return yuv_queue;
    }
private:
    ThreadSafeQueue<YUVFileData> *yuv_queue;

    //根据图像格式，计算出一帧图像的大小
    AVPixelFormat video_format;
    uint32_t buffer_size;
    uint8_t *out_buffer;
    SwsContext *sws_context;



    void getYUVData(AVFrame *in_frame, YUVFileData *yuv_frame_data);
};


#endif //AVPLAYER_AV_DECODER_VIDEO_H
