//
// Created by bo on 2022/2/21.
//
#include "header/Pusher.h"

#include <utility>
using namespace std;

void Pusher::start(std::string dest_url) {
    AVFormatContext *inputContext = initInputContext();
    av_dump_format(inputContext, 0, nullptr, 0);

    AVFormatContext *outputContext = initOutputContext(inputContext, dest_url);

    sendAVPacket(inputContext, outputContext, dest_url);

    avformat_free_context(outputContext);
    avformat_free_context(inputContext);
}

void Pusher::stop() {

}

AVFormatContext *Pusher::initOutputContext(AVFormatContext *inputContext, std::string destUrl) {
    AVFormatContext *outputContext = nullptr;
    avformat_alloc_output_context2(&outputContext, nullptr, "flv", destUrl.c_str());

    for (int i = 0; i < inputContext->nb_streams; ++i) {
        AVStream *input_stream = inputContext->streams[i];
        AVCodec *input_codec = avcodec_find_decoder(input_stream->codecpar->codec_id);
        if (input_codec == nullptr) {
            FFLoger::printErrorMessage("find codec failed.", errno);
            break;
        }
        AVStream *out_stream = avformat_new_stream(outputContext, input_codec);
        if (out_stream == nullptr) {
            FFLoger::printErrorMessage("init output stream failed.", errno);
            break;
        }

        int ret = avcodec_parameters_copy(out_stream->codecpar, input_stream->codecpar);
        if (ret < 0) {
            FFLoger::printErrorMessage("copy parameters failed.", ret);
            break;
        }

        AVCodecContext *codecContext = avcodec_alloc_context3(input_codec);
        avcodec_parameters_to_context(codecContext, input_stream->codecpar);

        codecContext->codec_tag = 0;
        if (codecContext->flags & AVFMT_GLOBALHEADER) {
            codecContext->codec_tag |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }
    av_dump_format(outputContext, 0, destUrl.c_str(), 1);

    return outputContext;
}

void Pusher::sendAVPacket(AVFormatContext *inputContext, AVFormatContext *outputContext, std::string destUrl) {
    int video_stream_index = -1;
    int audio_stream_index = -1;
    //Unit:microsecond
    uint32_t frame_interval_time = 3000;
    for (int i = 0; i < inputContext->nb_streams; ++i) {
        AVStream *input_stream = inputContext->streams[i];
        switch (input_stream->codecpar->codec_type) {
            case AVMEDIA_TYPE_AUDIO:
                audio_stream_index = i;
                break;
                case AVMEDIA_TYPE_VIDEO:
                    video_stream_index = i;
                    frame_interval_time = av_q2d(inputContext->streams[i]->r_frame_rate) * 1000;
                    cout << "frame_interval_time:" << frame_interval_time << endl;
                    break;
                    default:
                        break;
        }
    }

    //open output io
    int ret = -1;
    if (!(outputContext->flags & AVFMT_NOFILE)) { //非文件类输出
        ret = avio_open(&outputContext->pb, destUrl.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            FFLoger::printErrorMessage("open avio failed.", ret);
            return;
        }
    }

    //write header
    ret = avformat_write_header(outputContext, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("write header failed.", ret);
        return;
    }

    AVPacket *temp_packet = av_packet_alloc();

    uint64_t frame_index = 0;
    uint64_t start_time = av_gettime();
    while (av_read_frame(inputContext, temp_packet) >= 0) {
        //delay some time
        if (temp_packet->stream_index == video_stream_index) {
            uint64_t now = av_gettime();
            uint64_t interval_time = now - start_time;
            start_time = now;
            if (interval_time < frame_interval_time) {
                av_usleep(frame_interval_time - interval_time);
            }
            cout << "frame index:" << frame_index++ << endl;
        }

        int ret = av_interleaved_write_frame(outputContext, temp_packet);
        if (ret < 0) {
            FFLoger::printErrorMessage("failed to write frame.", ret);
            break;
        }
        av_packet_unref(temp_packet);
    }

    av_write_trailer(outputContext);
    avformat_close_input(&inputContext);
    if(!(outputContext->flags & AVFMT_NOFILE)) {
        avio_close(outputContext->pb);
    }
}
