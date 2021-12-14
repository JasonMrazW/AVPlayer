//
// Created by bo on 2021/12/14.
//

#ifndef AVPLAYER_SOCKET_TCP_SERVER_H
#define AVPLAYER_SOCKET_TCP_SERVER_H
#include "socket_server.h"
#include <iostream>

class TCPServer : public SocketServer {
public:
    TCPServer();
    ~TCPServer();

    void start() override;

    void stop() override;
};


#endif //AVPLAYER_SOCKET_TCP_SERVER_H
