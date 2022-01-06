//
// Created by bo on 2022/1/6.
//

#ifndef AVPLAYER_IPUSHER_H
#define AVPLAYER_IPUSHER_H
#include <iostream>

class IPusher {
public:
    IPusher() {}
    ~IPusher() {}

    //开始推流
    virtual void start(std::string dest_url) = 0;

    //停止推流
    virtual void stop() = 0;
};
#endif //AVPLAYER_IPUSHER_H
