//
// Created by bo on 2021/12/10.
//

#ifndef AVPLAYER_FLVAUDIOTAG_H
#define AVPLAYER_FLVAUDIOTAG_H
using namespace std;
typedef struct Audio_Tag {
    //header 1bit
    //0000 00 0 0
    //format rate size channel_type
    uint8_t format; // 4 bits
    uint8_t rate;// 2bits 采样率
    uint8_t size; // 采样深度 0=8-bit, 1=16-bit
    uint8_t channel_type; // 0 = mono, 1=stereo
    uint8_t aac_packet_type; //format = 10时，需要解析该字段，1 bit，0 = aac sequence header, 1 = aac raw data
    uint8_t *audio_data;//真实的音频数据
} AudioTag;

enum AUDIO_FORMAT {
    LINEAR_PCM_PLATFORM_ENDIAN = 0,
    ADPCM,
    MP3,
    LINEAR_PCM_LITTLE_ENDIAN,
    AAC = 10,
    SPEEX
    //其它非常规值，不做记录
};

enum AUDIO_SAMPLE_RATE {
    KHZ_5_5, //5.5 KHZ
    KHZ_11,
    KHZ_22,
    KHZ_44
};

enum AUDIO_SIZE {
    BIT_8, BIT_16
};

enum AUDIO_CHANNEL_TYPE {
    MONO, STEREO
};

enum AUDIO_AAC_PACKET_TYPE {
    AAC_SEQUENCE_HEADER, AAC_RAW
};

static string getAudioFormatDescription(uint8_t type) {
    string result;
    switch (type) {
        case LINEAR_PCM_PLATFORM_ENDIAN:
            result = "linear pcm platform endian";
            break;
        case ADPCM:
            result = "ADPCM";
            break;
        case MP3:
            result = "MP3";
            break;
        case LINEAR_PCM_LITTLE_ENDIAN:
            result = "linear pcm little endian";
            break;
        case AAC:
            result = "aac";
            break;
        case SPEEX:
            result = "speex";
            break;
        default:
            result = "unknown type:" + type;
    }

    return  result;
}

static string getSampleRate(uint8_t type) {
    string result;
    switch (type) {
        case KHZ_5_5:
            result = "5.5 khz";
            break;
        case KHZ_11:
                result = "11 khz";
            break;
        case KHZ_22:
                result = "22 khz";
                break;
        case KHZ_44:
                result = "44 khz";
                break;
        default:
                result = "unknown type:" + type;
    }

    return  result;
}
#endif //AVPLAYER_FLVAUDIOTAG_H
