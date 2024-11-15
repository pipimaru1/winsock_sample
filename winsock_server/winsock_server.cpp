#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <cstdio> // snprintf
#include <ws2tcpip.h>
#include "WinsockServer.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 4410

WinsockServer::WinsockServer()
{
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
}

int WinsockServer::open()
{
    // Winsockの初期化
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }
    else
        std::cerr << "WSAStartup success" << std::endl;

    // ソケット作成
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
    if (listen(listenSock, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    else
        std::cerr << "Listen success" << std::endl;

    std::cout << "Waiting for connection..." << std::endl;

    return 0;
}
////////////////////////////////////////////////////////////////////////////////
int WinsockServer::sklisten()
{
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
int WinsockServer::recievef3(float* data, size_t size, int flag)
{
    int bytesReceived = recv(clientSock, reinterpret_cast<char*>(data), size, flag);//ここで待機が発生
    return bytesReceived;
}

int WinsockServer::recieve_char32x16(char data[NUM_STRINGS][STRING_LENGTH])
{
    int bytesReceived = recv(clientSock, reinterpret_cast<char*>(data), NUM_STRINGS* STRING_LENGTH, 0);

    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
        return -1;
    }
    return bytesReceived;
}


////////////////////////////////////////////////////////////////////////////////
int WinsockServer::sockend() 
{
    closesocket(clientSock);
    std::cout << "Waiting for new connection..." << std::endl;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
int WinsockServer::close()
{
    closesocket(listenSock);
    WSACleanup();
    return 0;
}
