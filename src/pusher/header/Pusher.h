//
// Created by bo on 2022/1/6.
//

#ifndef AVPLAYER_PUSHER_H
#define AVPLAYER_PUSHER_H
#include <iostream>
#include "../../util/FFLoger.h"
extern "C" {
    #include "libavformat/avformat.h"
    #include "libavutil/avutil.h"
    #include "libavutil/time.h"
};

class Pusher {
public:
    Pusher() {}
    ~Pusher() {}

    //开始推流
    void start(std::string dest_url);

    //停止推流
    void stop();

protected:
    virtual AVFormatContext* initInputContext() = 0;

    AVFormatContext* initOutputContext(AVFormatContext *inputContext, std::string destUrl);

    void sendAVPacket(AVFormatContext *inputContext, AVFormatContext *outputContext, std::string destUrl);
};
#endif //AVPLAYER_PUSHER_H
