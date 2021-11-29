#include <iostream>
#include "SDLImagePlayer.hpp"
#include "parser/header/YUVImageParser.h"
#include "parser/header/RGBImageParser.h"
#include "SDLAudioPlayer.h"

int main() {
    std::cout << "start!" << sizeof (char) <<"||||||" << std::endl;
    CApp app;
//    IImageParser *parser = new RGBImageParser;
//    parser->loadFile();
//    return app.OnExecute(parser);
    SDLAudioPlayer audioPlayer;
    audioPlayer.play("resources/out.pcm");
    return 1;
}
