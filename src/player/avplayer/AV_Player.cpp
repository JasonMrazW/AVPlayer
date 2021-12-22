//
// Created by bo on 2021/12/18.
//

#include "header/AV_Player.h"

AV_Player::AV_Player() {
    render = new AV_Render_SDL();
    demuxer = new AVDemuxer();
}

AV_Player::~AV_Player() {

}

void AV_Player::start(std::string url) {
    std::thread demuxer_thread = thread(std::ref(loadDemuxerCallback), std::ref(*demuxer), url);

    render->start();
    demuxer_thread.join();
}

void AV_Player::loadDemuxerCallback(AVDemuxer &demuxer, std::string url) {
    demuxer.start(url.c_str());
}

