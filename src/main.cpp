#include <iostream>
#include "SDLImagePlayer.hpp"
#include "parser/header/YUVImageParser.h"
#include "parser/header/RGBImageParser.h"
#include "SDLAudioPlayer.h"
#include "parser/header/H264Parser.h"

int main() {
    std::cout << "start!" << sizeof (char) <<"||||||" << std::endl;
    std::cout << sizeof(unsigned int) << std::endl;
//    CApp app;
//    IImageParser *parser = new YUVImageParser;
//    parser->loadFile();
//    return app.OnExecute(parser);
//    SDLAudioPlayer audioPlayer;
//    audioPlayer.play("resources/out.pcm");
    H264Parser parser;
    parser.init();

    //1010
    uint8_t tmp = 0xa;
    uint8_t factor;
    //找到具体是哪位非0
    factor = 0x01;
    while ((tmp & factor) == 0 && tmp >= 0) {
        std::cout << (int)tmp << ",";
        tmp = tmp >> 1;
    }
    std::cout << "------" << (int)tmp << ",";
    tmp = tmp >> 1;
    std::cout << "------" << (int)tmp << ",";

    return 0;
}
