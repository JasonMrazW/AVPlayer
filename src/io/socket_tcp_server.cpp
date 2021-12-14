//
// Created by bo on 2021/12/14.
//

#include "socket_tcp_server.h"


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

    char client_ip[INET_ADDRSTRLEN] = "";
    while(!stoped) {
        std::clog << "listening.." << std::endl;
        conn = accept(socketFd, (sockaddr *)client_addr, &client_addr_len);
        if (conn < 0) {
            std::cerr << "accept socket failed." << errno << strerror(errno) << std::endl;
            continue;
        }

        inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, INET_ADDRSTRLEN);
        std::clog << "client ip..." << client_ip << ":" << ntohs(client_addr->sin_port) << std::endl;

        char buf[255];
        std::string result ="hello";
        while (true) {
            memset(buf, 0, sizeof(buf));
            int len = recv(conn, buf, sizeof (buf), 0);
            buf[len] = '\0';
            if (strcmp(buf, SOCKET_CONNECT_END.c_str()) == 0) {
                std::clog << "client disconnect..." << client_ip << std::endl;
                break;
            }
            std::clog << "receive from client:" << buf << std::endl;
            //send back to client
            send(conn, result.c_str(), result.length(), 0);
        }
        close(conn);
    }
}

void TCPServer::stop() {
    std::clog << "server stopping.." << std::endl;
    stoped = true;
    close(socketFd);
}

TCPServer::TCPServer() {
    std::clog << "constructor.." << std::endl;
}

TCPServer::~TCPServer() {

}