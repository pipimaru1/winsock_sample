#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <cstdio> // snprintf
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 4410
#define STRING_LENGTH 32
#define NUM_STRINGS 16

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

class WinsockServer
{
public:
    WSADATA wsaData;
    SOCKET listenSock;
    sockaddr_in serverAddr;
    SOCKET clientSock;

    int open();
    int sklisten();
    //int recieve();
    //int recieve(char* data, size_t size, int flag = 0);
    //int recieve(reinterpret_cast<char*> data, size_t size, int flag = 0);
    int recievef3(float* data, size_t size, int flag = 0); //float[3]を受け取る関数
    int recieve_char32x16(char data[NUM_STRINGS][STRING_LENGTH]);

    int sockend();
    int close();

    WinsockServer();
};

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
////////////////////////////////////////////////////////////////////////////////
int main() 
{
    //IPアドレス表示
    displayLocalIPAddress();
    
    /*
    // Winsockの初期化
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }
    else
        std::cerr << "WSAStartup success" << std::endl;


    // ソケット作成
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }
    else
        std::cerr << "Socket creation success" << std::endl;


    // サーバーアドレスの設定
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

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

    // 接続待ち
    if (listen(listenSock, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    else
        std::cerr << "Listen success" << std::endl;
    */

    WinsockServer wsv;
    wsv.open();

    std::cout << "Waiting for connection..." << std::endl;

    while (true) 
    {
        /*
        // クライアントからの接続を受け入れる
        SOCKET clientSock = accept(wsv.listenSock, NULL, NULL);
        if (clientSock == INVALID_SOCKET) 
        {
            std::cerr << "Accept failed." << std::endl;
            closesocket(wsv.listenSock);
            WSACleanup();
            return 1;
        }
        else
            std::cerr << "Accept success" << std::endl;
            */

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
            std::cout <<"ID: "<<data[0] << " Mouse Position: X=" << data[1] << " Y=" << data[2] << " Z=" << data[3] << std::endl;
        }
        // ソケットのクリーンアップ
        //closesocket(clientSock);
        //std::cout << "Waiting for new connection..." << std::endl;
        wsv.sockend();
    }

    wsv.close();
/*
    closesocket(listenSock);
    WSACleanup();
*/
    return 0;
}
