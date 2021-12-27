//
// Created by bo on 2021/12/20.
//

#include "header/AV_Demuxer.h"
using namespace std;

void AVDemuxer::start(const char * url) {
    AVState *current_state = new AVState();

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

    //clog << "id:" << current_state->video_codec->id << endl;
    //1秒25帧数据
    video_packet_queue = new ThreadSafeQueue<AVPacket>(25*4);
    //1秒 44100/1024个帧数据
    audio_packet_queue = new ThreadSafeQueue<AVPacket>(100*4);

    //启动图像解码器
    video_decoder = new AVDecoderVideo(current_state->format_context, current_state->video_stream, video_packet_queue);
    video_decode_thread = thread(std::ref(loadVideoDecoderThreadCallback),std::ref(video_decoder));

    //启动音频解码器
    audio_decoder = new AVDecoderAudio(current_state->format_context, current_state->audio_stream, audio_packet_queue);
    audio_decode_thread = thread(std::ref(loadAudioDecoderThreadCallback), std::ref(audio_decoder));

    readAVPackets(current_state->format_context, current_state);

    //stop read frame
    video_decoder->stop();
    video_decode_thread.detach();

    audio_decoder->stop();
    audio_decode_thread.detach();

    avcodec_close(current_state->video_codecContext);
    avcodec_close(current_state->audio_codecContext);
    close(current_state->format_context);
}

void AVDemuxer::readAVPackets(AVFormatContext *formatContext, AVState *state) {
    AVPacket *avPacket = av_packet_alloc();
    while(av_read_frame(formatContext, avPacket) >= 0) {
        if (avPacket->stream_index == state->video_stream_index) {
            video_packet_queue->enqueue(*avPacket);
        } else if(avPacket->stream_index == state->audio_stream_index) {
            audio_packet_queue->enqueue(*avPacket);
        } else {
            // do nothing
        }
    }
}

void AVDemuxer::close(AVFormatContext *formatContext) {
    avformat_close_input(&formatContext);
}

void AVDemuxer::loadVideoDecoderThreadCallback(AVDecoderVideo *video_decoder) {
    video_decoder->start();
}

void AVDemuxer::loadAudioDecoderThreadCallback(AVDecoderAudio *audio_decoder) {
    audio_decoder->start();
}
