#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <cstdio> // snprintf
#include <ws2tcpip.h>
#include "WinsockServer.h"


////////////////////////////////////////////////////////////////////////////////

// IPアドレスを取得して表示する関数
void displayLocalIPAddress() {
    char hostName[256];
    if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) {
        std::cerr << "Error: Unable to get host name." << std::endl;
        return;
    }

    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET; // IPv4のみ
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* result = nullptr;
    if (getaddrinfo(hostName, nullptr, &hints, &result) != 0) {
        std::cerr << "Error: Unable to get address info." << std::endl;
        return;
    }

    std::cout << "Server IP Addresses:" << std::endl;
    for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        std::cout << "  - " << ipAddress << std::endl;
    }

    freeaddrinfo(result);
}


////////////////////////////////////////////////////////////////////////////////
int main()
{
    //IPアドレス表示
    displayLocalIPAddress();

    WinsockServer wsv;
    wsv.open();

    std::cout << "Waiting for connection..." << std::endl;

    while (true)
    {

        wsv.sklisten();
        // データ受信ループ
        while (true)
        {
            //float data[3];
            //int bytesReceived = wsv.recievef3(data, sizeof(data));

            char data[NUM_STRINGS][STRING_LENGTH];
            int bytesReceived = wsv.recieve_char32x16(data);

            if (bytesReceived <= 0)
            {
                std::cerr << "Receive failed or connection closed." << std::endl;
                break;
            }

            // マウス座標を表示
            std::cout << "ID: " << data[0] << " Mouse Position: X=" << data[1] << " Y=" << data[2] << " Z=" << data[3] << std::endl;
        }
        wsv.sockend();
    }

    wsv.close();

    return 0;
}
