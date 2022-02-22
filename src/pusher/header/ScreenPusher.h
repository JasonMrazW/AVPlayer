//
// Created by bo on 2022/2/21.
//

#ifndef AVPLAYER_SCREENPUSHER_H
#define AVPLAYER_SCREENPUSHER_H
#include "Pusher.h"
#include "../../util/ContainerPrinter.h"
extern "C" {
    #include "libavdevice/avdevice.h"
};


class ScreenPusher : public Pusher{
public:
    ScreenPusher(){}
    ~ScreenPusher() {}

protected:
    AVFormatContext *initInputContext() override;

    AVFormatContext *initOutputContext(AVFormatContext *inputContext, std::string destUrl) override;

    void sendAVPacket(AVFormatContext *inputContext, AVFormatContext *outputContext, std::string destUrl) override;
};


#endif //AVPLAYER_SCREENPUSHER_H
