//
// Created by bo on 2021/11/29.
//
#include "header/RGBImageParser.h"

YUVFileData *RGBImageParser::loadFile() {
    YUVFileData* data = new YUVFileData;
    data->data = IImageParser::loadFile("resources/out_rgb24_900x1016.rgb", 900, 1016);
    data->width = 900;
    data->height = 1016;
    data->format = SDL_PIXELFORMAT_RGB24;
    data->pin = data->width * 3; //一行像素占的空间，单位：字节

    yuvFileData = data;

    toR(yuvFileData->data, data->width, data->height);
    return data;
}

/**
 * 仅保留R值
 * @param data
 * @param width
 * @param height
 */
void RGBImageParser::toR(uint8_t *data, int width, int height) {
    int length = width * height * 3;

    for (int i = 0; i < length; ++i) {
        if (i%3 == 0) continue;
        data[i] = 0;
    }
}

YUVFileData *RGBImageParser::init() {
    YUVFileData *fileData = new YUVFileData();
    fileData->width = 1080;
    fileData->height = 1920;
    fileData->format = SDL_PIXELFORMAT_RGB24;
    fileData->pin = fileData->width; //一行像素占的空间，单位：字节

    yuvFileData = fileData;
    return fileData;
}
