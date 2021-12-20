//
// Created by bo on 2021/12/20.
//

#include "header/AV_Demuxer.h"
using namespace std;

void AVDemuxer::init(const char * url) {
    AVFormatContext *format_context = nullptr;
    AVDictionary *dic = nullptr;
    int ret = avformat_open_input(&format_context, url, nullptr, &dic);
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
    uint8_t audio_stream_index = -1;

    for (int i = 0; i < format_context->nb_streams; ++i) {
        switch (format_context->streams[i]->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO:
                video_stream = format_context->streams[i];
                video_stream_index = i;
                break;
            case AVMEDIA_TYPE_AUDIO:
                audio_stream = format_context->streams[i];
                audio_stream_index = i;
                break;
            default:
                continue;
        }
    }

    //init video codec

    AVCodec *video_codec = nullptr;
    AVCodecContext *video_codecContext = nullptr;
    initCodec(video_stream, &video_codec, &video_codecContext);

    clog << "id:" << video_codec->id << endl;
    avformat_close_input(&format_context);
}

bool AVDemuxer::initCodec(AVStream *video_stream, AVCodec **out_codec, AVCodecContext **out_codecContext) {
    AVCodec *temp_codec= avcodec_find_decoder(video_stream->codecpar->codec_id);
    *out_codec = temp_codec;
    if (out_codec == nullptr) {
        cerr << "init codec failed." << strerror(errno) << endl;
        return false;
    }

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

void AVDemuxer::close() {

}
