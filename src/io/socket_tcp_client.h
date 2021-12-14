//
// Created by bo on 2021/12/14.
//

#ifndef AVPLAYER_SOCKET_TCP_CLIENT_H
#define AVPLAYER_SOCKET_TCP_CLIENT_H
#include "socket_client.h"
#include <iostream>

class SocketTcpClient: public SocketClient{
private:
    int socketFd;
public:
    SocketTcpClient();
    ~SocketTcpClient();

    void connectToServer() override;

    void disconnect() override;
};


#endif //AVPLAYER_SOCKET_TCP_CLIENT_H
