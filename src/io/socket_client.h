//
// Created by bo on 2021/12/14.
//

#ifndef AVPLAYER_SOCKET_CLIENT_H
#define AVPLAYER_SOCKET_CLIENT_H
#include "socket_info.h"

class SocketClient {
private:
    int socketFd;
public:
    SocketClient();
    virtual ~SocketClient();

    virtual void connectToServer() = 0;

    virtual void disconnect() = 0;
};


#endif //AVPLAYER_SOCKET_CLIENT_H
