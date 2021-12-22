//
// Created by bo on 2021/12/18.
//

#ifndef AVPLAYER_AV_PLAYER_H
#define AVPLAYER_AV_PLAYER_H
#include "AV_Render_SDL.h"
#include "AV_Demuxer.h"

class AV_Player {
public:
    AV_Player();
    ~AV_Player();

    void start(std::string url);
private:
    AV_Render_SDL *render;
    AVDemuxer *demuxer;

    static void loadDemuxerCallback(AVDemuxer &demuxer, std::string url);
};


#endif //AVPLAYER_AV_PLAYER_H
