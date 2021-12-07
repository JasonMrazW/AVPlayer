#include <iostream>
#include "player/SDLImagePlayer.hpp"
#include "parser/header/YUVImageParser.h"
#include "parser/header/RGBImageParser.h"
#include "player/SDLAudioPlayer.h"
#include "parser/header/H264Parser.h"
#include "parser/header/AACParser.h"
#include "parser/header/FLVParaser.h"
#include "util/BinaryUtil.h"

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

//    uint8_t *temp = new uint8_t [3];
//    //0001e2
//    //0000 0000--->0000 0000 0000 0000 0000 0000
//    //0000 0000 0000 0000 0000 0000->0000 0000 0000 0001 0000 0000
//    //0000 0000 0000 0001 0000 0000->0000 0000 0000 0001 1110 0010
//    temp[0] = 0x0;
//    temp[1] = 0x1;
//    temp[2] = 0xe2;
//
//    uint32_t  result = BinaryUtil::getUint32(temp, 3);
//    cout << result << endl;

    return 0;
}
