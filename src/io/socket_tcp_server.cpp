//
// Created by bo on 2021/12/14.
//

#include "socket_tcp_server.h"

void TCPServer::start() {
    std::clog << "start.." << std::endl;
}

void TCPServer::stop() {
    std::clog << "stop.." << std::endl;
}

TCPServer::TCPServer() {
    std::clog << "constructor.." << std::endl;
}

TCPServer::~TCPServer() {
    std::clog << "un constructor.." << std::endl;
}