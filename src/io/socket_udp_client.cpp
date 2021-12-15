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

    char input_data[255];
    char receive_buf[255];
    ssize_t receive_len = 0;
    sockaddr_in peer;
    ssize_t send_size = 0;
    while (!stoped) {
        cout << "please input your message:" << endl;
        memset(input_data, 0, sizeof (input_data));
        cin >> input_data;

        send_size = sendto(socket_fd, input_data, sizeof (input_data), 0, (sockaddr *)server_addr, server_addr_len);
        if (send_size < 0) {
            cout << "send failed: code->" << errno << "  " << strerror(errno) << endl;
            continue;
        }

        if (strcmp(input_data, SOCKET_CONNECT_END.c_str()) == 0) {
            cout << "quit send status." << endl;
            break;
        }
        memset(receive_buf, 0 , sizeof (receive_buf));
        receive_len = recvfrom(socket_fd, receive_buf, sizeof(receive_buf), 0, (sockaddr *)&peer, &server_addr_len);
        if (receive_len <=0) {
            cout << "receive data is empty." << endl;
            continue;
        }
        receive_buf[receive_len] = '\0';
        cout << "client: server says " << receive_buf << endl;
    }
}

void UDPClient::disconnect() {
    stoped = true;
    close(socket_fd);
}


