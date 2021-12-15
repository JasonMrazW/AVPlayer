//
// Created by bo on 2021/12/15.
//

#include "socket_udp_client.h"
using namespace std;

UDPClient::UDPClient() {

}

UDPClient::~UDPClient() {

}

void UDPClient::connectToServer() {
    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (socket_fd < 0) {
        cerr << "init socket failed." << errno << strerror(errno) << endl;
        return;
    }

    sockaddr_in *server_addr = new sockaddr_in();
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(8089);
    server_addr->sin_addr.s_addr = INADDR_ANY;

    socklen_t server_addr_len = sizeof (*server_addr);

    char receive_buf[255];
    ssize_t receive_len = 0;
    sockaddr_in peer;
    ssize_t send_size = 0;
    std::string send_buffer;
    while (!stoped) {
        cout << "udp client: please input your message:" << endl;
        cin >> send_buffer;

        send_size = sendto(socket_fd, send_buffer.c_str(), strlen(send_buffer.c_str()), 0, (sockaddr *)server_addr, server_addr_len);
        if (send_size < 0) {
            cout << "udp client: send failed: code->" << errno << "  " << strerror(errno) << endl;
            continue;
        }

        if (strcmp(send_buffer.c_str(), SOCKET_CONNECT_END.c_str()) == 0) {
            cout << "udp client: quit send status." << endl;
            break;
        }
        memset(receive_buf, 0 , sizeof (receive_buf));
        receive_len = recvfrom(socket_fd, receive_buf, sizeof(receive_buf), 0, (sockaddr *)&peer, &server_addr_len);
        if (receive_len <=0) {
            cout << "udp client:receive data is empty." << endl;
            continue;
        }
        receive_buf[receive_len] = '\0';
        cout << "udp client: server says " << receive_buf << endl;
    }
}

void UDPClient::disconnect() {
    stoped = true;
    close(socket_fd);
}


