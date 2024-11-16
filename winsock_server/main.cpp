#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <cstdio> // snprintf
#include <ws2tcpip.h>
#include <string>
#include <conio.h>  // Windows用キー入力監視 (_kbhit())

#include "WinsockServer.h"


////////////////////////////////////////////////////////////////////////////////
// IPアドレスを取得して表示する関数
//char ip_address[16][32]に出てきたipアドレスを格納する。15文字でいいんだけどね。nullを渡さないこと。
//32文字を16個まで格納して返す。
#define IPSTR_LENGTH    32
#define IPSTR_NUM       16
char IP_ADRRESS[IPSTR_NUM][IPSTR_LENGTH];
void displayLocalIPAddress(char ip_address[IPSTR_NUM][IPSTR_LENGTH])
{
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "Error: WSAStartup failed with error: " << wsaResult << std::endl;
        return;
    }

    char hostName[256];
    if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) 
    {
        std::cerr << "Error: Unable to get host name." << std::endl;
        return;
    }

    struct addrinfo hints = { 0 };
    hints.ai_family = AF_INET; // IPv4のみ
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* result = nullptr;
    if (getaddrinfo(hostName, nullptr, &hints, &result) != 0) 
    {
        std::cerr << "Error: Unable to get address info." << std::endl;
        WSACleanup();
        return;
    }

    int i = 0;
    std::cerr << "Server IP Addresses:" << std::endl;
    for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) 
    {
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        std::cerr << "  - " << ipAddress << std::endl;
        strcpy_s(ip_address[i], IPSTR_LENGTH, ipAddress);

        WSACleanup();
        i++;
    }

    freeaddrinfo(result);
    WSACleanup();
}


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    WinsockServer wsv;

    // コマンドライン引数のチェック
    if (argc < 3) 
    {
        std::cerr << "Usage: " << argv[0] << " <Waiting Port> <TCP or UDP>" << std::endl;
        std::cerr << "Use default Port = 4410, Protocol = TCP" << std::endl;
    }    // Winsockの初期化
    else
    {
        std::cerr << "Waiting Port = " << argv[1] << std::endl;
        std::cerr << "Protcol = " << argv[2] << std::endl;

        wsv.Port = std::stoi(argv[1]);

        // プロトコルの比較
        if (strcmp(argv[2], "UDP") == 0 || 
            strcmp(argv[2], "udp") == 0)
            wsv.Protocol = true;
        else
            wsv.Protocol = false;

        std::cerr << "Use default Port = " << wsv.Port;
            
        if(wsv.Protocol)
            std::cerr << ", Protocol = UDP" << std::endl;
        else
            std::cerr << ", Protocol = TCP" << std::endl;
    }

    //IPアドレス表示
    displayLocalIPAddress(IP_ADRRESS);

    wsv.open();

    std::cerr << "Waiting for connection..." << std::endl;

    while (true)
    {
        // キー入力があれば、終了処理
        if (_kbhit()) {
            std::cerr << "Key pressed, exiting..." << std::endl;
            break;
        }

        wsv.sklisten();
        // データ受信ループ
        while (true)
        {
            // キー入力があれば、終了処理
            if (_kbhit()) {
                std::cerr << "Key pressed, exiting..." << std::endl;
                wsv.sockend(); // 内部ループの終了時にもクリーンアップ
                goto end;       // 外側ループの終了
            }

            //受信
            int ret = wsv.receive();
            if (ret <= 0)
                break;
            // マウス座標を表示
            //std::cerr  << "ID: " << wsv.data[0] << " Mouse Position: X=" << wsv.data[1] << " Y=" << wsv.data[2] << " Z=" << wsv.data[3] << std::endl;
            std::cerr << "ID: " << wsv.data[0] << " Mouse Position: X=" << wsv.get_float(1) << " Y=" << wsv.get_float(2) << " Z=" << wsv.get_float(3) << std::endl;
        }
        wsv.sockend();
    }
end:
    wsv.close();
    return 0;
}
