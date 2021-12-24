//
// Created by bo on 2021/12/18.
//

#include "header/AV_Player.h"

AV_Player::AV_Player() {
    render = new AV_Render_SDL();
    render->init();
    demuxer = new AVDemuxer();
}

AV_Player::~AV_Player() {

}

//播放对应的地址
void AV_Player::start(std::string url) {
    std::thread demuxer_thread = thread(std::ref(loadDemuxerCallback), std::ref(*demuxer), url);

    //等待demux解码出第一帧
    while (demuxer->getYUVBuffer() == nullptr || demuxer->getPCMBuffer() == nullptr) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    //将缓冲区的指针给Render，让Render知道去哪取数据
    //render->setBuffer(demuxer->getYUVBuffer(), demuxer->getPCMBuffer());
    //render->start();

    //结束
    demuxer_thread.join();
}

void AV_Player::loadDemuxerCallback(AVDemuxer &demuxer, std::string url) {
    demuxer.start(url.c_str());
}

