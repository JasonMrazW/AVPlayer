#include <iostream>
#include "player/SDLImagePlayer.hpp"
#include "parser/header/YUVImageParser.h"
#include "parser/header/RGBImageParser.h"
#include "player/SDLAudioPlayer.h"
#include "parser/header/H264Parser.h"
#include "parser/header/AACParser.h"
#include "parser/header/FLVParaser.h"

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

//    AACParser aacParser;
//    aacParser.init();

    //FLVParaser flv_parser;
    FLVParaser::init();
    return 0;
}
