//
// Created by bo on 2021/12/22.
//

#ifndef AVPLAYER_AV_RENDER_RAWITEM_H
#define AVPLAYER_AV_RENDER_RAWITEM_H
struct PCMItem {
    uint8_t *data = nullptr;
    uint32_t data_length = 0;
    uint16_t audio_format;
    uint16_t nb_samples;
    uint32_t freq;
    uint8_t channels;
    double pts;
    double time_base;
};

struct YUVItem {
    uint8_t *data = nullptr; // 文件二进制内容
    int width; //分辨率宽
    int height; //分辨率高
    uint32_t format; //文件格式
    int pin;
    double pts;
    double time_base;
};
#endif //AVPLAYER_AV_RENDER_RAWITEM_H
