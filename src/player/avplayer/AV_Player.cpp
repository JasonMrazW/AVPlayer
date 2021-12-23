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

void AV_Player::start(std::string url) {
    std::thread demuxer_thread = thread(std::ref(loadDemuxerCallback), std::ref(*demuxer), url);

    while (demuxer->getYUVBuffer() == nullptr || demuxer->getPCMBuffer() == nullptr) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    render->setBuffer(demuxer->getYUVBuffer(), demuxer->getPCMBuffer());
    render->start();
    demuxer_thread.join();
}

void AV_Player::loadDemuxerCallback(AVDemuxer &demuxer, std::string url) {
    demuxer.start(url.c_str());
}

