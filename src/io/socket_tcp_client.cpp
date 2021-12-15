//
// Created by bo on 2021/12/14.
//

#include "socket_tcp_client.h"

TCPClient::TCPClient() {

}

TCPClient::~TCPClient() noexcept {
    disconnect();
}

void TCPClient::connectToServer() {
    std::clog << "tcp client start..." << std::endl;
    socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socketFd == -1) {
        std::cerr << "init failed." << errno << strerror(errno) << std::endl;
        return;
    }

    sockaddr_in *addr_in = new sockaddr_in();
    addr_in->sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_in->sin_port = htons(8088);
    addr_in->sin_family = AF_INET;
    if (connect(socketFd, (sockaddr*)addr_in, sizeof(*addr_in)) < 0) {
        std::cerr << "client connect failed." << errno << strerror(errno) << std::endl;
        return;
    }

    char data[255];
    char buf[255];
    while(!stoped) {
        std::cin >> data;
        send(socketFd, data, sizeof (data), 0);
        if (strcmp(data, SOCKET_CONNECT_END.c_str()) == 0) {
            std::clog << "client disconnect..." << std::endl;
            break;
        }
        memset(buf, 0, sizeof (buf));
        int len = recv(socketFd, buf, sizeof (buf), 0);
        buf[len] = '\0';
        std::cout << "receive from server:" << buf << std::endl;
    }

    delete addr_in;
}

void TCPClient::disconnect() {
    stoped = true;
    close(socketFd);
}
