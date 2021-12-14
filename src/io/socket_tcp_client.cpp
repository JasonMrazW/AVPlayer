//
// Created by bo on 2021/12/14.
//

#include "socket_tcp_client.h"

SocketTcpClient::SocketTcpClient() {

}

void SocketTcpClient::connectToServer() {
    std::clog << "client start..." << std::endl;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        std::cerr << "init failed." << errno << strerror(errno) << std::endl;
        return;
    }

    sockaddr_in *addr_in = new sockaddr_in();
    addr_in->sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_in->sin_port = htons(8088);
    addr_in->sin_family = AF_INET;
    if (connect(socketFd, (sockaddr*)addr_in, sizeof(addr_in)) < 0) {
        std::cerr << "client connect failed." << errno << strerror(errno) << std::endl;
        return;
    }


}

void SocketTcpClient::disconnect() {

}
