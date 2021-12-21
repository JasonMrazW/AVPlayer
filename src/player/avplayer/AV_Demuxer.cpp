//
// Created by bo on 2021/12/20.
//

#include "header/AV_Demuxer.h"
using namespace std;

void AVDemuxer::start(const char * url) {
    current_state = new AVState();

    int ret = avformat_open_input(&current_state->format_context, url, nullptr, nullptr);
    if (ret < 0) {
        cerr << "open file failed. error code :" << errno << " "<< strerror(errno) << endl;
        return;
    }

    ret = avformat_find_stream_info(current_state->format_context, nullptr);
    if (ret < 0) {
        cerr << "find stream info failed. error code :" << errno << " "<< strerror(errno) << endl;
        return;
    }
    av_dump_format(current_state->format_context, 0, nullptr, 0);

    //find video stream index and audio stream index

    for (int i = 0; i < current_state->format_context->nb_streams; ++i) {
        switch (current_state->format_context->streams[i]->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO:
                current_state->video_stream = current_state->format_context->streams[i];
                current_state->video_stream_index = i;
                break;
            case AVMEDIA_TYPE_AUDIO:
                current_state->audio_stream = current_state->format_context->streams[i];
                current_state->audio_stream_index = i;
                break;
            default:
                continue;
        }
    }

    //start video codec
    initCodec(current_state->video_stream, &current_state->video_codec, &current_state->video_codecContext);

    clog << "id:" << current_state->video_codec->id << endl;
}

bool AVDemuxer::initCodec(AVStream *video_stream, AVCodec **out_codec, AVCodecContext **out_codecContext) {
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

void readAVPackets(AVFormatContext *formatContext) {
    AVPacket *avPacket = av_packet_alloc();
    while(av_read_frame(formatContext, avPacket)) {

    }

    //close()
}

void AVDemuxer::close(AVFormatContext *formatContext) {
    avformat_close_input(&formatContext);
}
