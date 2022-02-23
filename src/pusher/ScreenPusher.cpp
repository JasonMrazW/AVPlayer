//
// Created by bo on 2022/2/21.
//

#include "header/ScreenPusher.h"
using namespace std;

AVFormatContext *ScreenPusher::initInputContext() {
    AVFormatContext *input_context = avformat_alloc_context();
    avdevice_register_all();
    AVDictionary *dictionary = nullptr;
    //av_dict_set(&dictionary, "list_devices", "true", 0);

    AVInputFormat *inputFormat = av_find_input_format("avfoundation");
    int ret = avformat_open_input(&input_context, "0:0", inputFormat, &dictionary);
    if (ret < 0) {
        FFLoger::printErrorMessage("failed to open input device.", ret);
    }

    ret = avformat_find_stream_info(input_context, nullptr);
    if (ret < 0) {
        FFLoger::printErrorMessage("failed to open input device.", ret);
    }

    return input_context;
}

AVFormatContext *ScreenPusher::initOutputContext(AVFormatContext *inputContext, std::string destUrl) {
    AVFormatContext *outputContext = nullptr;
    avformat_alloc_output_context2(&outputContext, nullptr, "flv", destUrl.c_str());
    return outputContext;
}

void ScreenPusher::sendAVPacket(AVFormatContext *inputContext, AVFormatContext *outputContext, std::string destUrl) {
    //set frame rate,1秒30帧,单位microseconds
    int ret = -1;
    uint64_t video_frame_interval_time = 1000*1000/30;
    uint64_t audio_frame_interval_time = 1000*1000*512/48000;

    uint32_t video_index = -1;
    uint32_t audio_index = -1;
    AVCodecContext *videoDecoderContext = nullptr;
    AVCodecContext *audioDecoderContext = nullptr;

    AVCodecContext *videoEncoderContext = nullptr;
    AVCodecContext *audioEncoderContext = nullptr;

    for (int i = 0; i < inputContext->nb_streams; ++i) {
        //初始化解码相关参数
        AVStream *stream = inputContext->streams[i];
        AVCodec *decoder = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext *decoderContext = avcodec_alloc_context3(decoder);
        avcodec_parameters_to_context(decoderContext, stream->codecpar);
        ret = avcodec_open2(decoderContext, decoder, nullptr);
        if (ret < 0) {
            FFLoger::printErrorMessage("failed to open decoder.", ret);
            return;
        }

        switch (stream->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO: {
                video_index = i;
                videoDecoderContext = decoderContext;
                videoEncoderContext = initEncoder(outputContext, stream);

                break;
            }
            case AVMEDIA_TYPE_AUDIO: {
                audio_index = i;
                audioDecoderContext = decoderContext;
                audioEncoderContext = initEncoder(outputContext, stream);
                break;
            }
            default: {
                //
            }
        }
    }
    av_dump_format(outputContext, 0, destUrl.c_str(), 1);

    //write header
    if (!(outputContext->flags & AVFMT_NOFILE)) {
        ret = avio_open(&outputContext->pb, destUrl.c_str(), AVIO_FLAG_WRITE);
        if(ret < 0) {
            FFLoger::printErrorMessage("failed to open av io.", ret);
            return;
        }
    }

    ret = avformat_write_header(outputContext, nullptr);
    if(ret < 0) {
        FFLoger::printErrorMessage("failed to open av io.", ret);
        return;
    }


    //****************************************************************************************
    //video
    //初始化编解码所需数据
    AVPacket *inputAVPacket = av_packet_alloc();
    AVFrame *inputAVFrame = av_frame_alloc();

    AVPacket *outputAVPacket = av_packet_alloc();
    AVFrame *outputAVFrame = av_frame_alloc();

    AVPixelFormat inputFormat = static_cast<AVPixelFormat>(inputContext->streams[video_index]->codecpar->format);
    AVPixelFormat outputFormat = AV_PIX_FMT_YUV420P;

    outputAVFrame->format = outputFormat;

    uint32_t output_buffer_size = av_image_get_buffer_size(outputFormat, videoEncoderContext->width, videoEncoderContext->height, 1);
    uint8_t *output_buffer = static_cast<uint8_t *>(av_malloc(output_buffer_size));

    av_image_fill_arrays(outputAVFrame->data, outputAVFrame->linesize, output_buffer, outputFormat,
                         videoEncoderContext->width, videoEncoderContext->height, 1);

    AVCodecParameters *inputCodecParameters = inputContext->streams[video_index]->codecpar;
    SwsContext *swsContext = sws_getContext(inputCodecParameters->width, inputCodecParameters->height, inputFormat,
                                            videoEncoderContext->width, videoEncoderContext->height, outputFormat,
                                            SWS_BICUBIC, nullptr, nullptr, nullptr);

    //****************************************************************************************
    //audio
    AVFrame *audioOutputAVFrame = av_frame_alloc();
    AVFrame *audioInputAVFrame = av_frame_alloc();
    AVPacket *audioOutputAVPacket = av_packet_alloc();

    AVSampleFormat audioOutputSampleFormat = AV_SAMPLE_FMT_FLTP;
    audioOutputAVFrame->format = AV_SAMPLE_FMT_FLTP;
    audioOutputAVFrame->channels = 1;
    audioOutputAVFrame->channel_layout = AV_CH_LAYOUT_MONO;
    audioOutputAVFrame->nb_samples = 512;
    audioOutputAVFrame->sample_rate = 48000;

    AVCodecParameters *inputAudioCodecParam = inputContext->streams[audio_index]->codecpar;
    AVSampleFormat audioInputSampleFormat = static_cast<AVSampleFormat>(inputAudioCodecParam->format);

    SwrContext *swrContext = swr_alloc_set_opts(nullptr, AV_CH_LAYOUT_MONO, audioOutputSampleFormat,
                                                audioEncoderContext->sample_rate,
                                                inputAudioCodecParam->channel_layout, audioInputSampleFormat,
                                                inputAudioCodecParam->sample_rate, 0, nullptr);

    ret = swr_init(swrContext);
    if (ret < 0) {
        FFLoger::printErrorMessage("failed to init swr context.", ret);
        return;
    }

    uint32_t buffer_size = av_samples_get_buffer_size(nullptr, inputAudioCodecParam->channels, 512,
                                                      audioOutputSampleFormat, 1);
    uint8_t *audioOutBuffer = static_cast<uint8_t *>(av_malloc(buffer_size));
    memset(audioOutBuffer, 0, buffer_size);
    av_samples_fill_arrays(audioOutputAVFrame->data, audioOutputAVFrame->linesize, audioOutBuffer,
                           audioEncoderContext->channels, 512, audioOutputSampleFormat, 1);

    //********
    int frame_index = 0;
    ret = 0;
    bool running = true;
    uint64_t video_pts = 0;
    uint64_t audio_pts = 0;
    uint64_t video_time = 0;
    uint64_t audio_time = 0;

    double video_time_base = av_q2d(inputContext->streams[video_index]->time_base);
    double audio_time_base = av_q2d(inputContext->streams[audio_index]->time_base);

    uint64_t pre_video_dts = 0;
    uint64_t pre_audio_dts = 0;

    while(running) {
        ret = av_read_frame(inputContext, inputAVPacket);
        if (ret < 0) {
            av_usleep(200);
            continue;
        }

//        cout << "frame index:" << frame_index++ << endl;
        if (inputAVPacket->stream_index == video_index) {
//            continue;
            ret = avcodec_send_packet(videoDecoderContext, inputAVPacket);
            if (ret < 0) {
                FFLoger::printErrorMessage("failed to decode.", ret);
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(videoDecoderContext, inputAVFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    //receive frame end
                    break;
                }

                //convert uyuv422 to yuv420p
                sws_scale(swsContext, inputAVFrame->data, inputAVFrame->linesize, 0, inputAVFrame->height,
                          outputAVFrame->data, outputAVFrame->linesize);

                //outputAVFrame即为转换成YUV420P之后的数据
                outputAVFrame->width = inputAVFrame->width;
                outputAVFrame->height = inputAVFrame->height;
                outputAVFrame->pts = inputAVFrame->pts;
                outputAVFrame->pkt_dts = inputAVFrame->pkt_dts;

                //送入编码器
                ret = avcodec_send_frame(videoEncoderContext, outputAVFrame);
                if (ret < 0) {
                    FFLoger::printErrorMessage("send video frame failed.", ret);
                    break;
                }
                while (ret >= 0) {
                    //从编码器取编码后的数据
                    ret = avcodec_receive_packet(videoEncoderContext, outputAVPacket);

                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        //receive frame end
                        break;
                    }
                    if (outputAVPacket->pts != AV_NOPTS_VALUE)
                        outputAVPacket->pts = av_rescale_q(outputAVPacket->pts,
                                                           inputContext->streams[inputAVPacket->stream_index]->time_base,
                                                           outputContext->streams[inputAVPacket->stream_index]->time_base);
                    if (outputAVPacket->dts != AV_NOPTS_VALUE)
                        outputAVPacket->dts = av_rescale_q(outputAVPacket->dts,
                                                           inputContext->streams[inputAVPacket->stream_index]->time_base,
                                                           outputContext->streams[inputAVPacket->stream_index]->time_base);
                    outputAVPacket->stream_index = inputAVPacket->stream_index;
                    pre_video_dts = outputAVPacket->dts;

                    ret = av_interleaved_write_frame(outputContext, outputAVPacket);
                    if (ret < 0) {
                        FFLoger::printErrorMessage("failed to write video frame.", ret);
                        continue;
                    }

                    cout << "vide frame index:" << frame_index++ << endl;
                }

                av_frame_unref(inputAVFrame);
            }
        } else {
//            continue;
            ret = avcodec_send_packet(audioDecoderContext, inputAVPacket);
            if (ret < 0) {
                FFLoger::printErrorMessage("failed to decode.", ret);
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(audioDecoderContext, audioInputAVFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                }

                int samples = swr_convert(swrContext, &audioOutBuffer, audioInputAVFrame->nb_samples,
                                          (const uint8_t **)audioInputAVFrame->extended_data, audioInputAVFrame->nb_samples);
                if (samples <= 0) {
                    cerr << "convert audio info failed." << endl;
                    break;
                }

                audioOutputAVFrame->pts = audioInputAVFrame->pts;
                audioOutputAVFrame->pkt_size = audioInputAVFrame->nb_samples * 4;

                //送入编码器
                ret = avcodec_send_frame(audioEncoderContext, audioOutputAVFrame);
                if (ret < 0) {
                    FFLoger::printErrorMessage("failed to encode audio frame.", ret);
                    continue;
                }

                while (ret >= 0) {
                    ret = avcodec_receive_packet(audioEncoderContext, audioOutputAVPacket);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        //receive frame end
                        break;
                    }
                    if (audioOutputAVPacket->pts != AV_NOPTS_VALUE)
                        audioOutputAVPacket->pts = av_rescale_q(audioOutputAVPacket->pts,
                                                           inputContext->streams[inputAVPacket->stream_index]->time_base,
                                                           outputContext->streams[inputAVPacket->stream_index]->time_base);
                    if (audioOutputAVPacket->dts != AV_NOPTS_VALUE)
                        audioOutputAVPacket->dts = av_rescale_q(audioOutputAVPacket->dts,
                                                           inputContext->streams[inputAVPacket->stream_index]->time_base,
                                                           outputContext->streams[inputAVPacket->stream_index]->time_base);

                    //audioOutputAVPacket->duration = audioOutputAVPacket->pts - pre_audio_dts;
                    audioOutputAVPacket->stream_index = inputAVPacket->stream_index;
                    pre_audio_dts = audioOutputAVPacket->pts;

                    ret = av_interleaved_write_frame(outputContext, audioOutputAVPacket);
                    if (ret < 0) {
                        FFLoger::printErrorMessage("failed to write audio frame.", ret);
                        continue;
                    }
                }
            }

        }
        av_packet_unref(inputAVPacket);
    }

    cout << "end......" << endl;
}

AVCodecContext * ScreenPusher::initEncoder(AVFormatContext *outputContext, AVStream *stream) {
    AVCodecContext *encoderContext  = nullptr;
    //初始化编码相关参数
    if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
        encoderContext = avcodec_alloc_context3(encoder);
        encoderContext->framerate = {30, 1};
        encoderContext->time_base = {1, 30};
        encoderContext->width = stream->codecpar->width;
        encoderContext->height = stream->codecpar->height;
        encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;
        encoderContext->gop_size = 60;
        encoderContext->codec_id = AV_CODEC_ID_H264;
        encoderContext->codec_type = AVMEDIA_TYPE_VIDEO;
        encoderContext->max_b_frames = 2;
        av_opt_set(encoderContext->priv_data, "preset", "slow", 0);

        int ret = avcodec_open2(encoderContext, encoder, nullptr);
        if (ret < 0) {
            FFLoger::printErrorMessage("failed to open video encoder.", ret);
            return nullptr;
        }

        AVStream *outStream = avformat_new_stream(outputContext, encoder);
        outStream->codecpar->bit_rate = 2000*1000;
        outStream->codecpar->format = AV_PIX_FMT_YUV420P;
        outStream->codecpar->width = stream->codecpar->width;
        outStream->codecpar->height = stream->codecpar->height;
        outStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        outStream->codecpar->codec_id = AV_CODEC_ID_H264;
    } else {
        AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
        encoderContext = avcodec_alloc_context3(encoder);
        encoderContext->sample_rate = 48000;
        encoderContext->profile = FF_PROFILE_AAC_HE;
        encoderContext->time_base = {1, 48000};
        encoderContext->sample_fmt = encoder->sample_fmts[0];
        encoderContext->channels = 1;
        encoderContext->channel_layout = AV_CH_LAYOUT_MONO;
        encoderContext->codec_id = AV_CODEC_ID_AAC;
        encoderContext->codec_type = AVMEDIA_TYPE_AUDIO;
        encoderContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

        int ret = avcodec_open2(encoderContext, encoder, nullptr);
        if (ret < 0) {
            FFLoger::printErrorMessage("failed to open audio encoder.", ret);
            return nullptr;
        }

        AVStream *outStream = avformat_new_stream(outputContext, encoder);
        outStream->codecpar->bit_rate = stream->codecpar->bit_rate;
        outStream->codecpar->sample_rate = 48000;
        outStream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        outStream->codecpar->codec_id = AV_CODEC_ID_AAC;
        outStream->codecpar->format = encoder->sample_fmts[0];
        outStream->codecpar->channels = 1;
        outStream->codecpar->channel_layout = AV_CH_LAYOUT_MONO;
        outStream->codecpar->frame_size = 512;
    }

    encoderContext->codec_tag = 0;
    if (encoderContext->flags & AVFMT_GLOBALHEADER) {
        encoderContext->codec_tag |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    return encoderContext;
}
