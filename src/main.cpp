#include <iostream>
#include "SDLImagePlayer.hpp"
#include "parser/header/YUVImageParser.h"
#include "parser/header/RGBImageParser.h"
#include "SDLAudioPlayer.h"
#include "parser/header/H264Parser.h"
#include "parser/header/AACParser.h"

using namespace std;

int main() {
    std::cout << "start!" << sizeof (char) <<"||||||" << std::endl;

//    CApp app;
//    IImageParser *parser = new YUVImageParser;
//    parser->loadFile();
//    return app.OnExecute(parser);

//    SDLAudioPlayer audioPlayer;
//    audioPlayer.play("resources/out.pcm");

//    H264Parser parser;
//    parser.init();

    AACParser aacParser;
    aacParser.init();

    //80 0a 1f
    //1000 0000 0000 1010 0001 1111
    //00 0000 1010 000

    uint16_t frame_size = 0;
    frame_size |= (0x80 & 0x3) << 11;
    frame_size |= 0x0a << 3;
    frame_size |= (0x1F & 0xE0) >> 5;

    //cout << frame_size << endl;

    return 0;
}
