//
// Created by bo on 2021/12/16.
//

#include "ff_main_sample.h"

using namespace std;

static const char *const FF_URL = "https://d1--cn-gotcha04.bilivideo.com/live-bvc/919638/live_2087957890_69851677_1500.flv?expires=1639713368&len=0&oi=460523204&pt=web&qn=0&trid=100032c68ce8736a41c6ad6ebafe6c9355c4&sigparams=cdn,expires,len,oi,pt,qn,trid&cdn=cn-gotcha04&sign=48477b7f9684e62e61b98a069024e106&p2p_type=1&src=9&sl=1&free_type=0&flowtype=1&machinezone=ylf&sk=2935686d6cb9146c7a6a6a0b4e120e250342be3df4dc8310261aab0ce9e21e44&source=onetier&order=1";

FFMainSample::~FFMainSample() {

}

FFMainSample::FFMainSample(SDLImagePlayer *p) {
    player = p;
}

void FFMainSample::initContext() {
    AVFormatContext *format_context = nullptr;
    AVDictionary *dic = nullptr;
    int ret = avformat_open_input(&format_context, FF_URL, nullptr, &dic);
    if (ret < 0) {
        cerr << "open file failed. error code :" << errno << " "<< strerror(errno) << endl;
        return;
    }

    ret = avformat_find_stream_info(format_context, &dic);
    if (ret < 0) {
        cerr << "find stream info failed. error code :" << errno << " "<< strerror(errno) << endl;
        return;
    }
    av_dump_format(format_context, 0, nullptr, 0);

    //find video stream index and audio stream index
    AVStream *video_stream = nullptr;
    AVStream *audio_stream = nullptr;
    uint8_t video_stream_index = -1;


    for (int i = 0; i < format_context->nb_streams; ++i) {
        switch (format_context->streams[i]->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO:
                video_stream = format_context->streams[i];
                video_stream_index = i;
                break;
            case AVMEDIA_TYPE_AUDIO:
                audio_stream = format_context->streams[i];
                break;
            default:
                continue;
        }
    }

    //decode video frame
    decodeVideo(video_stream, format_context, video_stream_index);

    avformat_close_input(&format_context);
}

 void FFMainSample::printAVDictionary(AVDictionary *dic) {
    AVDictionaryEntry *entry = nullptr;
    int count = av_dict_count(dic);
    cout << "stream dic count:" << count << endl;
     for (int i = 0; i < count; ++i) {
         entry = av_dict_get(dic, "", entry, AV_DICT_IGNORE_SUFFIX);
         cout << entry->key << ":" << entry->value << endl;
     }
}

void FFMainSample::decodeVideo(AVStream *video_stream, AVFormatContext *format_context, uint8_t video_stream_index) {
    AVCodec *codec = avcodec_find_decoder(video_stream->codecpar->codec_id);

    if (codec == nullptr) {
        cerr << "init video codec failed" << endl;
        return;
    }

    AVCodecContext *codec_context = avcodec_alloc_context3(codec);
    //需要给codec context赋值，否则会报错找不到NALU的start code
    avcodec_parameters_to_context(codec_context, video_stream->codecpar);
    int ret = avcodec_open2(codec_context, codec, nullptr);
    if (ret < 0) {
        cerr << "open video codec failed. error code :" << errno << " "<< strerror(errno) << endl;
        return;
    }


    AVPacket *av_packet = av_packet_alloc();
    AVFrame *av_frame = av_frame_alloc();
    AVFrame *yuv_frame = av_frame_alloc();
    YUVFileData *yuv_frame_data = new YUVFileData();

    player->reInit(codec_context->width, codec_context->height);

    //根据图像格式，计算出一帧图像的大小
    AVPixelFormat video_format = static_cast<AVPixelFormat>(video_stream->codecpar->format);
    uint32_t buffer_size = av_image_get_buffer_size(video_format, codec_context->width, codec_context->height, 1);
    uint8_t *out_buffer = static_cast<uint8_t *>(av_malloc(buffer_size));
    //填充默认数据

    SwsContext *sws_context = sws_getContext(codec_context->width, codec_context->height,video_format,
                                             codec_context->width, codec_context->height,video_format,
                                             SWS_BICUBIC, nullptr,nullptr,nullptr);

    while(av_read_frame(format_context, av_packet) >= 0) {
        if (av_packet->stream_index != video_stream_index) {
            continue;
        }
        //clog << av_packet.pts << " | "<< av_packet.size <<  " | " << endl;
        ret = avcodec_send_packet(codec_context, av_packet);
        if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            cerr << "codec send packed failed. error code :" << ret << endl;
            break;
        }

        while(ret >= 0) {
            ret = avcodec_receive_frame(codec_context, av_frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                //receive frame end
                break;
            }
            
            yuv_frame = av_frame_alloc();
            av_image_fill_arrays(yuv_frame->data, yuv_frame->linesize, out_buffer, video_format, codec_context->width, codec_context->height,1);
            sws_scale(sws_context,(const uint8_t * const*)av_frame->data,av_frame->linesize,0,av_frame->height,yuv_frame->data,yuv_frame->linesize);

            yuv_frame_data->data = out_buffer;
            yuv_frame_data->pin = av_frame->width;

            player->updateYUVFileData(*yuv_frame_data);

            av_frame_unref(yuv_frame);
            av_frame_unref(av_frame);
        }
        av_packet_unref(av_packet);
    }

    av_packet_unref(av_packet);
    av_frame_unref(av_frame);
    av_frame_unref(yuv_frame);
    sws_freeContext(sws_context);
    avcodec_close(codec_context);
}

// AVFrame To YUV
//            uint8_t *yuv_buffer;
//            buffer_size = av_frame->width * av_frame->height * sizeof (uint8_t) * 3;
//            //copy Y
//            for (int i = 0; i < av_frame->height; ++i) {
//                memcpy(yuv_buffer+av_frame->width*i,
//                       av_frame->data[0] + av_frame->linesize[0]*i,
//                       av_frame->width);
//            }
//
//            //copy U
//            int first_bytes = av_frame->width *av_frame->height;
//            for (int i = 0; i < av_frame->height/2; ++i) {
//                memcpy(yuv_buffer+ first_bytes + av_frame->width*i/2,
//                       av_frame->data[1] + av_frame->linesize[1]*i,
//                       av_frame->width/2);
//            }
//
//            //copy V
//            first_bytes += first_bytes/4;
//            for (int i = 0; i < av_frame->height/2; ++i) {
//                memcpy(yuv_buffer + first_bytes + av_frame->width *i/2,
//                       av_frame->data[2] + av_frame->linesize[2]*i,
//                       av_frame->width/2);
//            }
