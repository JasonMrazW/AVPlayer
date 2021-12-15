//
// Created by bo on 2021/12/14.
//

#ifndef AVPLAYER_SOCKET_SERVER_H
#define AVPLAYER_SOCKET_SERVER_H
#include <sys/socket.h>
#include <iostream>
#include "socket_info.h"
#include <thread>

class SocketServer {
public:
    SocketServer();
    virtual ~SocketServer() = 0;

    virtual void start() = 0;

    virtual void stop() = 0;
};




#endif //AVPLAYER_SOCKET_SERVER_H
