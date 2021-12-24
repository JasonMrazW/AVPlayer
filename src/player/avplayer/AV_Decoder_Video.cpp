//
// Created by bo on 2021/12/21.
//

#include "header/AV_Decoder_Video.h"

void AVDecoderVideo::stop() {
    running = false;
}

void AVDecoderVideo::getYUVData(AVFrame *in_frame, YUVItem *yuv_frame_data) {
    AVFrame *yuv_frame = av_frame_alloc();
    av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, out_buffer, video_format, codec_context->width, codec_context->height,1);
    sws_scale(sws_context,in_frame->data,in_frame->linesize,0,in_frame->height,yuv_frame->data,yuv_frame->linesize);

    yuv_frame_data->data = out_buffer;
    yuv_frame_data->width = in_frame->width;
    yuv_frame_data->height = in_frame->height;
    yuv_frame_data->format = ConvertUtil::AVPixFormatToSDLPixelFormat(static_cast<AVPixelFormat>(in_frame->format));
    yuv_frame_data->pin = in_frame->width;

    av_frame_unref(yuv_frame);
}

//解码器吐出来的AVFrame是按显示顺序吐的，非解码顺序
void AVDecoderVideo::parseAVFrame(AVFrame *av_frame) {
    double time_base = av_q2d(av_stream->time_base);
    double codec_time_base = av_q2d(codec_context->time_base);
    cout << "dts:" << av_frame->pkt_dts << endl;
    cout << "pts:" << av_frame->pkt_pts << endl;

//    cout << "pts:" << double (av_frame->pts * time_base) << endl;
//    cout << "pkg_pts" << double(av_frame->pkt_pts * time_base) << endl;
    //get yuv data & add to yuv buffer
    YUVItem temp;
    getYUVData(av_frame, &temp);
    //送数据进buffer
    yuv_queue->enqueue(temp);
}


