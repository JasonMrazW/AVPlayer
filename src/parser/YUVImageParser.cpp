//
// Created by bo on 2021/11/29.
//

#include "header/YUVImageParser.h"
const int tmp_file_width = 970;
const int tmp_file_height = 583;
const char * tmp_filePath = "resources/out_bg_yuv420p.yuv";

YUVImageParser::YUVImageParser() {

}
 YUVFileData *YUVImageParser::loadFile() {
    YUVFileData *fileData = new YUVFileData();
    fileData->data = IImageParser::loadFile(tmp_filePath, tmp_file_width, tmp_file_height);

    toGray(fileData->data, tmp_file_width, tmp_file_height);

    fileData->width = tmp_file_width;
    fileData->height = tmp_file_height;
    fileData->format = SDL_PIXELFORMAT_IYUV;
    fileData->pin = fileData->width; //一行像素占的空间，单位：字节

    yuvFileData = fileData;
    return fileData;
}

YUVFileData *YUVImageParser::init() {
    YUVFileData *fileData = new YUVFileData();
    fileData->data = IImageParser::loadFile(tmp_filePath, tmp_file_width, tmp_file_height);;
    fileData->width = 1920;
    fileData->height = 1080;
    fileData->format = SDL_PIXELFORMAT_IYUV;
    fileData->pin = fileData->width; //一行像素占的空间，单位：字节

    yuvFileData = fileData;
    return fileData;
}

/**
 * 420p数据组织格式
 * Y:1个字节，U：0.5个字节，V:0.5个字节
 * planer组织模式，所有的Y，然后是U
 * @param fileContent
 * @param width
 * @param height
 */
uint8_t * YUVImageParser::toGray(uint8_t *fileContent, int width, int height) {
    // Y分量所占得大小：width * height
    int y_length = width * height;
    // U、V 分量共同占有得大小：width * height
    int uv_length = width * height;

    int fileLength = -1;
    for (int i = y_length; i < fileLength; ++i) {
        //至为128即为初始值
        fileContent[i] = 128;
    }

    return fileContent;
}

/**
 * 调整YUV图片的亮度
 * @param fileContent
 * @param width
 * @param height
 * @param factor
 * @return
 */
uint8_t *YUVImageParser::changeLumaForYUV420P(uint8_t *fileContent, int width, int height, float factor) {
    int y_length = width * height;
    for (int i = 0; i < y_length; ++i) {
        fileContent[i] = fileContent[i] * factor;
    }
    return fileContent;
}

/**
 * 添加简单的边框效果
 * @param fileContent
 * @param width
 * @param height
 * @param border
 * @return
 */
uint8_t *YUVImageParser::addBorderForYUV420P(uint8_t *fileContent, int width, int height, int border) {
    //逐行扫描
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {

            if (i<border || i > (height-border) || j < border || j > (width-border)) {
                fileContent[i*width + j] = 255;
            }
        }
    }
    return fileContent;
}
