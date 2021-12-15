//
// Created by bo on 2021/12/14.
//

#ifndef AVPLAYER_SOCKET_UDP_SERVER_H
#define AVPLAYER_SOCKET_UDP_SERVER_H
#include "socket_server.h"

class UDPServer :  public SocketServer{
private:
    int socket_fd;
    bool stoped = false;
public:
    UDPServer();
    ~UDPServer() override;

    void start() override;

    void stop() override;
};


#endif //AVPLAYER_SOCKET_UDP_SERVER_H
