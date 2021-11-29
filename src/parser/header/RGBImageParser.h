//
// Created by bo on 2021/11/29.
//

#ifndef AVPLAYER_RGBIMAGEPARSER_H
#define AVPLAYER_RGBIMAGEPARSER_H
#include "IImageParser.h"
#include <SDL_pixels.h>

class RGBImageParser : public IImageParser{
public:
    RGBImageParser() {

    };

    ~RGBImageParser() {

    }

    virtual YUVFileData* loadFile();

    void toR(char *data, int width, int height);
};




#endif //AVPLAYER_RGBIMAGEPARSER_H
