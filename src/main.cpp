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
    return 0;
}
