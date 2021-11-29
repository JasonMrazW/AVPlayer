#include <iostream>
#include "SDLActivity.hpp"
#include "parser/header/YUVImageParser.h"
#include "parser/header/RGBImageParser.h"

int main() {
    std::cout << "start!" << sizeof (char) <<"||||||" << std::endl;
    CApp app;
    IImageParser *parser = new RGBImageParser;
    parser->loadFile();
    return app.OnExecute(parser);
}
