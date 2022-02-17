//
// Created by bo on 2021/12/16.
//

#ifndef AVPLAYER_FF_MAIN_SAMPLE_H
#define AVPLAYER_FF_MAIN_SAMPLE_H
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
};
#include <iostream>
#include "../parser/header/IImageParser.h"
#include "../parser/header/YUVImageParser.h"
#include "../player/SDLImagePlayer.hpp"
#include "../player/SDLAudioStreamPlayer.h"
#include <thread>
#include "../util/ConvertUtil.h"

class FFMainSample {
private:
    SDLImagePlayer *player;
    SDLAudioStreamPlayer *audio_player;
public:
    FFMainSample(SDLImagePlayer *player, SDLAudioStreamPlayer *audioPlayer);
    ~FFMainSample();

    void initContext();

    void printAVDictionary(AVDictionary *dic);

    void decodeVideo(AVStream *video_stream, AVFormatContext *format_context, uint8_t video_stream_index);

    void decodeAudio(AVStream *audio_stream, AVFormatContext *format_context, uint8_t audio_stream_index);
};


#endif //AVPLAYER_FF_MAIN_SAMPLE_H
