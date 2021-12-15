//
// Created by bo on 2021/12/14.
//

#include "socket_udp_server.h"

static const std::string UDP_FILE_PATH = "resources/video/temp_udpfile.rtp";

UDPServer::UDPServer() {

}

UDPServer::~UDPServer()  {
    stop();
}

void UDPServer::start() {
    std::clog << "udp server start..." << std::endl;

    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (socket_fd < 0) {
        std::cerr << "init socket failed." << errno << strerror(errno) << std::endl;
        return;
    }

    sockaddr_in *addr_in = new sockaddr_in();
    addr_in->sin_family = AF_INET;
    addr_in->sin_port = htons(8089);
    addr_in->sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (sockaddr*)addr_in, sizeof (*addr_in)) == -1) {
        std::cerr << "bind socket failed." << errno << strerror(errno) << std::endl;
        return;
    }


    socklen_t client_addr_len;
    uint8_t buf[2000]; //比mtu大一点
    ssize_t ret;
    const std::string result ="ok!";
    char client_ip[INET_ADDRSTRLEN] = "";

    std::ofstream out_stream;
    out_stream.open(UDP_FILE_PATH, std::ios::out | std::ios::binary);

    parser = new RTPParser();

    while (!stoped) {
        sockaddr_in client_addr;
        client_addr_len = sizeof(client_addr);
        memset(buf, 0, sizeof (buf));
        std::clog << "udp server listening..." << std::endl;
        ret = recvfrom(socket_fd, buf, sizeof(buf), 0, (sockaddr *)&client_addr, &client_addr_len);
        if (ret <= 0) {
            std::cerr << "receive failed: receive failed." << errno << strerror(errno) << std::endl;
            continue;
        }
        std::clog << "udp server : data size---->" << ret << std::endl;
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        //std::clog << "udp server : client ip..." << client_ip << ":" << ntohs(client_addr.sin_port)  << " is sending data!" << std::endl;

        //std::clog << "udp server: client says " << buf << std::endl;
        if (ret == 4 && strcmp((char*)buf, SOCKET_CONNECT_END.c_str()) == 0) {
            std::clog << "udp server: wait 1 seconds to exit." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            out_stream.close();
            continue;
        }

        out_stream << buf;

        //解析RTP数据包
        parser->parseOnePacket(buf, ret);

        //send back
        //sendto(socket_fd, result.c_str(), result.length(), 0, (sockaddr*)&client_addr, client_addr_len);
    }

    delete addr_in;
    delete parser;
}

void UDPServer::stop() {
    std::clog << "udp server: stop..." << std::endl;
    stoped = true;
    close(socket_fd);
}
