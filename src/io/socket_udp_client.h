//
// Created by bo on 2021/12/15.
//

#ifndef AVPLAYER_SOCKET_UDP_CLIENT_H
#define AVPLAYER_SOCKET_UDP_CLIENT_H
#include "socket_client.h"

class UDPClient: public  SocketClient{
private:
    int socket_fd;
    bool stoped;
public:
    UDPClient();
    ~UDPClient() override;

    void connectToServer() override;

    void disconnect() override;
};


#endif //AVPLAYER_SOCKET_UDP_CLIENT_H
