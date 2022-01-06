//
// Created by bo on 2022/1/6.
//

#ifndef AVPLAYER_LOCALFILEPUSHER_H
#define AVPLAYER_LOCALFILEPUSHER_H
#include "IPusher.h"
#include "../../util/FFLoger.h"
extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
};

class LocalFilePusher: public IPusher {
public:
    LocalFilePusher() {

    };
    ~LocalFilePusher() {

    }

private:
    void start(std::string dest_url) override;

    void stop() override;

    void readAVPacket(AVFormatContext *inputContext, std::string dest_url);

    AVFormatContext * initOutputFormat(const AVFormatContext *inputContext, const std::string &dest_url) const;
};


#endif //AVPLAYER_LOCALFILEPUSHER_H
