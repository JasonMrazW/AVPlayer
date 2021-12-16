//
// Created by bo on 2021/12/16.
//

#include "ff_main_sample.h"

using namespace std;

static const char *const LOCAL_FF_PATH = "resources/video/sample.flv";

FFMainSample::~FFMainSample() {

}

FFMainSample::FFMainSample() {
}

void FFMainSample::initContext() {
    AVFormatContext *format_context = nullptr;
    AVDictionary *dic = nullptr;
    int ret = avformat_open_input(&format_context, LOCAL_FF_PATH, nullptr, &dic);
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
    YUVFileData *yuv_frame_data = nullptr;
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

            clog << "frame count:" << codec_context->frame_number << endl;

            av_frame_unref(av_frame);
        }
        av_packet_unref(av_packet);
    }

    av_packet_unref(av_packet);
    av_frame_unref(av_frame);
    avcodec_close(codec_context);
}
