//
// Created by bo on 2021/12/21.
//

#include "header/AV_Decoder_Video.h"


void AVDecoderVideo::start() {
    running = true;
    AVPacket *av_packet = av_packet_alloc();
    AVFrame *av_frame = av_frame_alloc();

    int ret;
    while (isCodecInited&&running) {
        //send pack to decoder
        av_packet_queue->dequeue(*av_packet);
        ret = avcodec_send_packet(codec_context,av_packet);
        if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            cerr << "codec send packed failed. error code :" << ret << endl;
            break;
        }
        //receive frame from decoder
        while(ret >= 0) {
            ret = avcodec_receive_frame(codec_context, av_frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                //receive frame end
                break;
            }

            //get yuv data & add to yuv buffer
            YUVFileData temp;
            getYUVData(av_frame, &temp);
            //送数据进buffer
            std::cout << "yuv data enqueue" << std::endl;
            yuv_queue->enqueue(temp);
        }
    }
}

void AVDecoderVideo::stop() {
    running = false;
}

void AVDecoderVideo::getYUVData(AVFrame *in_frame, YUVFileData *yuv_frame_data) {
    AVFrame *yuv_frame = av_frame_alloc();
    av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, out_buffer, video_format, codec_context->width, codec_context->height,1);
    sws_scale(sws_context,in_frame->data,in_frame->linesize,0,in_frame->height,yuv_frame->data,yuv_frame->linesize);

    yuv_frame_data->data = out_buffer;
    yuv_frame_data->width = in_frame->width;
    yuv_frame_data->format = ConvertUtil::AVPixFormatToSDLPixelFormat(static_cast<AVPixelFormat>(in_frame->format));
    yuv_frame_data->pin = in_frame->width;
}


