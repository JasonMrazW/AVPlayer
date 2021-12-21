//
// Created by bo on 2021/12/17.
//

#include "ConvertUtil.h"

SDL_AudioFormat ConvertUtil::AVSampleFormatToSDLAudioFormat(AVSampleFormat in_format) {
    SDL_AudioFormat out_format;
    switch (in_format) {
        case AV_SAMPLE_FMT_NONE:
        case AV_SAMPLE_FMT_U8:          ///< unsigned 8 bits
            out_format = AUDIO_U8;
            break;
        case AV_SAMPLE_FMT_S16:         ///< signed 16 bits
            out_format = AUDIO_S16LSB;
            break;
        case AV_SAMPLE_FMT_S32:         ///< signed 32 bits
            out_format = AUDIO_S32;
            break;
        case AV_SAMPLE_FMT_FLT:         ///< float
            out_format = AUDIO_F32;
            break;
        case AV_SAMPLE_FMT_DBL:         ///< double
        case AV_SAMPLE_FMT_U8P:         ///< unsigned 8 bits: planar
        case AV_SAMPLE_FMT_S16P:        ///< signed 16 bits: planar
        case AV_SAMPLE_FMT_S32P:        ///< signed 32 bits: planar
        case AV_SAMPLE_FMT_FLTP:        ///< float: planar
        case AV_SAMPLE_FMT_DBLP:        ///< double: planar
        case AV_SAMPLE_FMT_S64:         ///< signed 64 bits
        case AV_SAMPLE_FMT_S64P:        ///< signed 64 bits: planar
        case AV_SAMPLE_FMT_NB:           ///< Number of sample formats. DO NOT USE if linking dynamically
        default:
            break;
    }
    return out_format;
}

SDL_PixelFormatEnum ConvertUtil::AVPixFormatToSDLPixelFormat(AVPixelFormat in_format) {
    SDL_PixelFormatEnum out_format = SDL_PIXELFORMAT_IYUV;
    switch (in_format) {
        case AV_PIX_FMT_YUV420P:
            out_format = SDL_PIXELFORMAT_IYUV;
            break;
        case AV_PIX_FMT_NV12:
            out_format = SDL_PIXELFORMAT_NV12;
            break;
        case AV_PIX_FMT_NV21:
            out_format = SDL_PIXELFORMAT_NV21;
            break;
        default:
            std::cout << "unsupported in format:" << in_format << std::endl;
            break;
    }
    return out_format;
}
