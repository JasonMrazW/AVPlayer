#include <iostream>
#include <vector>
#include <ctime>
#include "player/SDLImagePlayer.hpp"
#include "parser/header/YUVImageParser.h"
#include "parser/header/RGBImageParser.h"
#include "player/SDLAudioStreamPlayer.h"
#include "parser/header/H264Parser.h"
#include "parser/header/AACParser.h"
#include "parser/header/FLVParaser.h"
#include "basic/basic.h"
#include "io/socket_server.h"
#include "io/socket_tcp_server.h"
#include "io/socket_client.h"
#include "io/socket_tcp_client.h"
#include "io/socket_udp_client.h"
#include "io/socket_udp_server.h"
#include "ffmpeg/ff_main_sample.h"
#include "player/avplayer/header/AV_Render_SDL.h"
#include "player/avplayer/header/AV_Demuxer.h"
#include "thread/ThreadSafeQueue.h"
#include "player/avplayer/header/AV_Player.h"
#include "pusher/header/IPusher.h"
#include "pusher/header/LocalFilePusher.h"

extern "C" {
    #include <libavformat/avformat.h>
}


using namespace std;
SocketServer *server;


std::thread startServer() {
    return std::thread([]() {
        server = new UDPServer();
        server->start();
        delete server;
    });
}

std::thread startClient() {
    return std::thread([]() {
        SocketClient *client = new UDPClient();
        client->connectToServer();
        client->disconnect();

        delete client;
    });
}

void loadAudioPlayer(SDLAudioStreamPlayer &audioPlayer) {
    audioPlayer.startAudioPlayer();
}

void loadFFmpeg(SDLImagePlayer &player, SDLAudioStreamPlayer &audioPlayer) {
    FFMainSample mainSample(&player, &audioPlayer);
    mainSample.initContext();
}

void enqueue(uint8_t index, ThreadSafeQueue<int> &queue) {
    queue.enqueue(index);
    std::cout << "enqueue___" << (unsigned )index <<std::endl;
}

void dequeue(ThreadSafeQueue<int> &queue) {
    int value = 0;
    queue.dequeue(value);
}

void startPlayer() {
    AV_Player player;
    player.start("resources/video/sound_sync_test.flv");
}

void startPusher() {
    IPusher *pusher = new LocalFilePusher();
    pusher->start("rtmp://localhost:1935/live/rfBd56ti2SMtYvSgD5xAV0YU99zampta7Z7S575KLkIZ9PYk");
    pusher->stop();
}

int main() {
    std::cout << "start!" << sizeof (char) <<"||||||" << std::endl;

    //start sdl render
//    AV_Render_SDL render;
//    render.start();

//    startPlayer();

//    startPusher();

//    cout << last_pts << endl;
    //start image player
    SDLImagePlayer player;
    IImageParser *parser = new RGBImageParser();
    parser->loadFile();
    player.OnExecute(parser);
//
//    //start audio player
//    SDLAudioStreamPlayer audioPlayer;
//    std::thread thread(loadFFmpeg, std::ref(player), std::ref(audioPlayer));
////    audioPlayer.startAudioPlayer();
//    std::thread audio_thread(loadAudioPlayer, std::ref(audioPlayer));
//
//    //player.OnExecute(parser);
////    ffmpeg_thread.join();
////    audio_thread.join();

    //registerSignal();
//    createThreadsByPThead();
//    std::clog << "main program exit2" << std::endl;
//    createThread();

//    std::ffmpeg_thread server_thread = startServer();
//    std::ffmpeg_thread client_thread = startClient();
//
//    client_thread.join();
//    server->stop();
//    server_thread.join();


//    CApp app;
//    IImageParser *parser = new YUVImageParser;
//    parser->loadFile();
//    return app.OnExecute(parser);

//    SDLAudioPlayer audioPlayer;
//    audioPlayer.play("resources/out.pcm");

//    H264Parser parser;
//    parser.loadFromFile();

//    AACParser aacParser;
//    aacParser.start();

//    FLVParaser flv_parser;
//    FLVParaser::start();

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


