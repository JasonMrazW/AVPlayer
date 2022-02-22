//
// Created by bo on 2022/1/6.
//

#ifndef AVPLAYER_LOCALFILEPUSHER_H
#define AVPLAYER_LOCALFILEPUSHER_H
#include "Pusher.h"


class LocalFilePusher: public Pusher {
public:
    LocalFilePusher() {

    };
    ~LocalFilePusher() {

    }

protected:
    AVFormatContext *initInputContext() override;

    AVFormatContext *initOutputContext(AVFormatContext *inputContext, std::string destUrl) override;

    void sendAVPacket(AVFormatContext *inputContext, AVFormatContext *outputContext, std::string destUrl) override;
};


#endif //AVPLAYER_LOCALFILEPUSHER_H
