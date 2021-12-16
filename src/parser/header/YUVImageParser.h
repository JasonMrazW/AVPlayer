//
// Created by bo on 2021/11/29.
//

#ifndef AVPLAYER_YUVIMAGEPARSER_H
#define AVPLAYER_YUVIMAGEPARSER_H


#include "IImageParser.h"
#include <SDL_pixels.h>

class YUVImageParser : public IImageParser{
public:
    YUVImageParser();

    virtual YUVFileData *loadFile();

    uint8_t *      toGray(uint8_t *fileContent, int width, int height);

    /**
     * YUV文件亮度调整
     * @param fileContent
     * @param width
     * @param height
     * @return
     */
    uint8_t*        changeLumaForYUV420P(uint8_t *fileContent, int width, int height, float factor);

    /**
     * 添加简单的边框效果
     * @param fileContent
     * @param width
     * @param height
     * @param border
     * @return
     */
    uint8_t*        addBorderForYUV420P(uint8_t *fileContent, int width, int height, int border);


    YUVFileData *init();
};

#endif //AVPLAYER_YUVIMAGEPARSER_H
