#include <iostream>
#include <vector>
#include <ctime>
#include "player/SDLImagePlayer.hpp"
#include "parser/header/YUVImageParser.h"
#include "parser/header/RGBImageParser.h"
#include "player/SDLAudioPlayer.h"
#include "parser/header/H264Parser.h"
#include "parser/header/AACParser.h"
#include "parser/header/FLVParaser.h"
#include "basic/basic.h"

using namespace std;

class Books {
private:
    int len;
public:
    Books(int l);
    Books(const Books &books);
};

Books::Books(int l) : len(l){
    clog << "constructor be called." << endl;
}

Books::Books(const Books &books) {
    len = books.len;
    clog << "len is " << len << endl;
}

int main() {
    std::cout << "start!" << sizeof (char) <<"||||||" << std::endl;

    double a = 1.1;
    double b = 12.1;
    clog << maxNum(a, b) << endl;

    clog << __LINE__ << endl;
    clog << __DATE__ << endl;

    //registerSignal();
    createThreadsByPThead();
    std::clog << "main program exit2" << std::endl;
    createThread();

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

//    FLVParaser flv_parser;
//    FLVParaser::init();

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
    //4094 0000 0000 0000
    //0100 0000 1001 0100 0000 0000 0000 ...
//    uint8_t *temp = new uint8_t [8];
//    temp[0] = 0x40;
//    temp[1] = 0x94;
//
//    double result = BinaryUtil::getDouble(temp,8);
//    cout << "result:" << result << endl;

    return 0;
}


