//
// Created by bo on 2021/12/14.
//

#include "socket_tcp_server.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

void TCPServer::start() {
    std::clog << "start.." << std::endl;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        std::cerr << "init socket failed." << errno << strerror(errno) << std::endl;
        return;
    }

    sockaddr_in *addr = new sockaddr_in();
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(8088);

    if (bind(socketFd, (sockaddr*)addr, sizeof(*addr)) == -1) {
        std::cerr << "bind socket failed." << errno << strerror(errno) << std::endl;
        return;
    }

    if (listen(socketFd, 5) == -1) {
        std::cerr << "listen socket failed." << errno << strerror(errno) << std::endl;
        return;
    }

    int conn;
    sockaddr_in *client_addr = new sockaddr_in();
    socklen_t client_addr_len = sizeof (*client_addr);
    while(!stoped) {
        conn = accept(socketFd, (sockaddr *)client_addr, &client_addr_len);
        if (conn < 0) {
            std::cerr << "accept socket failed." << errno << strerror(errno) << std::endl;
            return;
        }
    }
}

void TCPServer::stop() {
    std::clog << "stop.." << std::endl;
    stoped = true;
    close(socketFd);
}

TCPServer::TCPServer() {
    std::clog << "constructor.." << std::endl;
}

TCPServer::~TCPServer() {
    std::clog << "un constructor.." << std::endl;
}