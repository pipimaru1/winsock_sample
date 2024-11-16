#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <cstdio> // snprintf
#include <ws2tcpip.h>
#include <string>
#include "WinsockServer.h"

#pragma comment(lib, "ws2_32.lib")

//#define PORT 4410

WinsockServer::WinsockServer():
    wsaData(),
    listenSock(),
    serverAddr(),
    clientSock(0)
{
    Port = (u_short)4410;
    Protocol = false;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(Port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
}

int WinsockServer::open()
{
    // Portの設定
    serverAddr.sin_port = htons(Port);

    // Winsockの初期化
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }
    else
        std::cerr << "WSAStartup success" << std::endl;

    // ソケット作成
    if(Protocol)//UDP
        listenSock = socket(AF_INET, SOCK_DGRAM, 0);
    else //TCP
        listenSock = socket(AF_INET, SOCK_STREAM, 0);


    if (listenSock == INVALID_SOCKET) 
    {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }
    else
        std::cerr << "Socket creation success" << std::endl;

    // ソケットをバインド
    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed." << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    else
        std::cerr << "Bind success" << std::endl;

    // 接続待ちに入る
    if (!Protocol)//TCP
    {
        if (listen(listenSock, 1) == SOCKET_ERROR) {
            std::cerr << "Listen failed." << std::endl;
            closesocket(listenSock);
            WSACleanup();
            return 1;
        }
        else
            std::cerr << "Listen success" << std::endl;
    }

    std::cerr << "Waiting for connection..." << std::endl;

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
int WinsockServer::sklisten()
{
    if (Protocol)//UDP リッスン不要
        return 0;

    // クライアントからの接続を受け入れる
    clientSock = accept(listenSock, NULL, NULL);
    if (clientSock == INVALID_SOCKET)
    {
        std::cerr << "Accept failed." << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    else
        std::cerr << "Accept success" << std::endl;

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
int WinsockServer::sockend()
{
    closesocket(clientSock);
    std::cerr << "Waiting for new connection..." << std::endl;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
int WinsockServer::close()
{
    closesocket(listenSock);
    WSACleanup();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//int WinsockServer::receivef3(float* _data, size_t size, int flag)
//{
//    int bytesReceived = recv(clientSock, reinterpret_cast<char*>(_data), size, flag);//ここで待機が発生
//    return bytesReceived;
//}

int WinsockServer::receive_char32x16(char _data[NUM_STRINGS][STRING_LENGTH])
{
    if (Protocol)//UDP リッスン不要
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        //recvfrom()で使用するソケットは、UDPの場合はlistenSockです。clientSockを使わないようにします。
        //int bytesReceived = recvfrom(clientSock, reinterpret_cast<char*>(_data), NUM_STRINGS * STRING_LENGTH, 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
        int bytesReceived = recvfrom(listenSock, reinterpret_cast<char*>(_data), NUM_STRINGS * STRING_LENGTH, 0, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
            return -1;
        }
        return bytesReceived;
    }
    else
    {
        int bytesReceived = recv(clientSock, reinterpret_cast<char*>(_data), NUM_STRINGS * STRING_LENGTH, 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
            return -1;
        }
        return bytesReceived;
    }
}

////////////////////////////////////////////////////////////////////////////////
int WinsockServer::receive()
{
    return receive_char32x16(data);
}


////////////////////////////////////////////////////////////////////////////////
float WinsockServer::get_float(size_t num)
{
    float value = std::stof(data[num]);
    return value;
}

double WinsockServer::get_double(size_t num)
{
    double value = std::stod(data[num]);
    return value;
}

int WinsockServer::get_int(size_t num)
{
    int value = std::stoi(data[num]);
    return value;
}
