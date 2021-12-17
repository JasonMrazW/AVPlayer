//
// Created by bo on 2021/12/17.
//

#ifndef AVPLAYER_CONVERTUTIL_H
#define AVPLAYER_CONVERTUTIL_H
#include <iostream>
#include <SDL_audio.h>
#include <libavutil/samplefmt.h>

class ConvertUtil {
public:
    static SDL_AudioFormat AVSampleFormatToSDLAudioFormat(AVSampleFormat in_format);
};


#endif //AVPLAYER_CONVERTUTIL_H
