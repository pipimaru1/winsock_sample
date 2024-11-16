#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <cstdio> // snprintf
#include <ws2tcpip.h>
#include <conio.h>  // Windows用キー入力監視 (_kbhit())

#pragma comment(lib, "ws2_32.lib")

//#define SERVER_IP "10.3.142.146"  // パソコンBのIPアドレスに変更
//#define PORT 4410
u_short Port = 4410;

#define STRING_LENGTH 32
#define NUM_STRINGS 16

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
    std::cout << "Server IP Addresses:" << std::endl;
    for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next)
    {
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        std::cout << "  - " << ipAddress << std::endl;
        strcpy_s(ip_address[i], IPSTR_LENGTH, ipAddress);

        WSACleanup();
        i++;
    }

    freeaddrinfo(result);
    WSACleanup();
}



void float_to_char32(float x, char str[32]) 
{
    // `snprintf` で float を文字列に変換
    snprintf(str, 32, "%.6f", x); // 小数点以下6桁まで表示
}

//32文字の文字列を16個送る関数 TCP
int send_char32x16(SOCKET sock, char data[NUM_STRINGS][STRING_LENGTH]) 
{
    int totalSize = STRING_LENGTH * NUM_STRINGS;
    if (send(sock, reinterpret_cast<char*>(data), totalSize, 0) == SOCKET_ERROR) 
    {
        std::cerr << "Send failed." << std::endl;
        return 1;
    }
    return 0;
}

//32文字の文字列を16個送る関数 UDP
int send_char32x16_udp(SOCKET sock, const char* ipAddress, u_short port, char data[NUM_STRINGS][STRING_LENGTH])
{
    int totalSize = STRING_LENGTH * NUM_STRINGS;

    sockaddr_in clientAddr;
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(port);
    int clientAddrSize = sizeof(clientAddr);

    // IP アドレスを設定
    //if (inet_pton(AF_INET, IP_ADRRESS[0], &clientAddr.sin_addr) <= 0) {
    if (inet_pton(AF_INET, ipAddress, &clientAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address format" << std::endl;
        return -1;
    }

    int bytesSent = sendto(
        sock, 
        reinterpret_cast<char*>(data), totalSize, 
        0, 
        (struct sockaddr*)&clientAddr, 
        clientAddrSize);

    return bytesSent;
}

int main(int argc, char* argv[]) 
{
    // コマンドライン引数のチェック
    if (argc < 4) 
    {
        std::cerr << "Usage: " << argv[0] << " <Server IP Address> <Port> <TCP or UDP> <polling time[msec]>" << std::endl;
        return 1;
    }    // Winsockの初期化

    //引数解析
    std::string serverIP = argv[1];
    u_short Port = (u_short)atoi(argv[2]);
    bool Protocol = false;
    // プロトコルの比較
    if (strcmp(argv[3], "UDP") == 0 ||
        strcmp(argv[3], "udp") == 0)
        Protocol = true;
    else if (strcmp(argv[3], "TCP") == 0 ||
             strcmp(argv[3], "tcp") == 0)
        Protocol = false;
    else //引数異常
    {
        std::cerr << "Usage: " << argv[0] << " <Server IP Address> <Port> <TCP or UDP> <polling time[msec]>" << std::endl;
        return 1;
    }
    int polling_time = atoi(argv[4]);

    //引数解析結果表示
    std::cout << "Server IP Adress = " << serverIP << std::endl;
    std::cout << "Port = " << Port << std::endl;
    std::cout << "Protocol(TCP:0/UDP:1) = " << Protocol << std::endl;
    std::cout << "Polling Time = " << polling_time << std::endl;

    //IPアドレス表示
    displayLocalIPAddress(IP_ADRRESS);


    if (!Protocol) //TCPの処理
    {
        // 接続ループ
        while (true)
        {
            // キー入力があれば、終了処理 winsock処理開始前なので何もせずbreak
            if (_kbhit()) {
                std::cout << "Key pressed, exiting..." << std::endl;
                break;
            }

            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cout << "WSAStartup failed" << std::endl;
                return 1;
            }
            else
                std::cout << "WSAStartup success" << std::endl;

            // ソケット作成
            SOCKET sock;
            sock = socket(AF_INET, SOCK_STREAM, 0);

            if (sock == INVALID_SOCKET) {
                std::cout << "Socket creation failed" << std::endl;
                WSACleanup();
                //return 1;
                continue;
            }
            else
                std::cout << "Socket creation success" << std::endl;

            // サーバーアドレスの設定
            sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(Port);
            inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

            // サーバーに接続
            if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
            {
                std::cout << "Connection failed" << std::endl;
                closesocket(sock);
                WSACleanup();
                Sleep(polling_time); // 待機
                std::cout << "Waiting Server: " << serverIP << std::endl;
                //return 1;
                continue;
            }
            else
                std::cout << "Connection success" << std::endl;

            // データの準備
            char data[NUM_STRINGS][STRING_LENGTH] = {
                "String 1",  "String 2",  "String 3",  "String 4",
                "String 5",  "String 6",  "String 7",  "String 8",
                "String 9",  "String 10", "String 11", "String 12",
                "String 13", "String 14", "String 15", "String 16"
            };

            // マウス座標の送信ループ
            while (true)
            {
                // キー入力があれば、終了処理
                if (_kbhit()) {
                    closesocket(sock);
                    std::cout << "closesocket" << std::endl;
                    WSACleanup();
                    std::cout << "WSACleanup" << std::endl;
                    goto end;       // 外側ループの終了
                }

                POINT mousePos;
                GetCursorPos(&mousePos); // マウスの現在位置を取得
                strcpy_s((char*)data[0], STRING_LENGTH, "CLIENT_001");
                float_to_char32((float)mousePos.x, data[1]);
                float_to_char32((float)mousePos.y, data[2]);
                float_to_char32(0.0f, data[3]);

                //送信処理
                int ret = send_char32x16(sock, data);

                //送信がうまくいかなかったら送信ループを出る
                if (ret)
                    break;

                //送信がうまくいったら送信内容を表示
                std::cout << "data[" << NUM_STRINGS << "]=";
                for (int k = 0; k < NUM_STRINGS; k++)
                {
                    std::cout << data[k] << ", ";
                }
                std::cout << std::endl;

                // 指定時間待機
                Sleep(polling_time);
            }
            // ソケットのクリーンアップ
            closesocket(sock);
            std::cout << "closesocket" << std::endl;
            WSACleanup();
            std::cout << "WSACleanup" << std::endl;
        }
    }
    else //UDP
    {
        // 接続ループ
        while (true)
        {
            // キー入力があれば、終了処理 winsock処理開始前なので何もせずbreak
            if (_kbhit()) {
                std::cout << "Key pressed, exiting..." << std::endl;
                break;
            }

            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cout << "WSAStartup failed" << std::endl;
                return 1;
            }
            else
                std::cout << "WSAStartup success" << std::endl;

            // ソケット作成
            SOCKET sock;
            sock = socket(AF_INET, SOCK_DGRAM, 0);

            if (sock == INVALID_SOCKET) {
                std::cout << "Socket creation failed" << std::endl;
                WSACleanup();
                //return 1;
                continue;
            }
            else
                std::cout << "Socket creation success" << std::endl;

            //// サーバーアドレスの設定
            //sockaddr_in serverAddr;
            //serverAddr.sin_family = AF_INET;
            //serverAddr.sin_port = htons(Port);
            //inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

            //// サーバーに接続
            //if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
            //{
            //    std::cout << "Connection failed" << std::endl;
            //    closesocket(sock);
            //    WSACleanup();
            //    Sleep(polling_time); // 待機
            //    std::cout << "Waiting Server: " << serverIP << std::endl;
            //    //return 1;
            //    continue;
            //}
            //else
            //    std::cout << "Connection success" << std::endl;

            // データの準備
            char data[NUM_STRINGS][STRING_LENGTH] = {
                "String 1",  "String 2",  "String 3",  "String 4",
                "String 5",  "String 6",  "String 7",  "String 8",
                "String 9",  "String 10", "String 11", "String 12",
                "String 13", "String 14", "String 15", "String 16"
            };

            // マウス座標の送信ループ
            while (true)
            {
                // キー入力があれば、終了処理
                if (_kbhit()) {
                    closesocket(sock);
                    std::cout << "closesocket" << std::endl;
                    WSACleanup();
                    std::cout << "WSACleanup" << std::endl;
                    goto end;       // 外側ループの終了
                }

                POINT mousePos;
                GetCursorPos(&mousePos); // マウスの現在位置を取得
                strcpy_s((char*)data[0], STRING_LENGTH, "CLIENT_001");
                float_to_char32((float)mousePos.x, data[1]);
                float_to_char32((float)mousePos.y, data[2]);
                float_to_char32(0.0f, data[3]);

                //送信処理
                int ret = send_char32x16_udp(sock, serverIP.c_str(), Port, data);

                //送信がうまくいかなかったら送信ループを出る
                if (ret)
                {
                    Sleep(polling_time);
                    break;
                }
                else //送信がうまくいったら送信内容を表示
                {
                    std::cout << "data[" << NUM_STRINGS << "]=";
                    for (int k = 0; k < NUM_STRINGS; k++)
                    {
                        std::cout << data[k] << ", ";
                    }
                    std::cout << std::endl;
                    // 指定時間待機
                    Sleep(polling_time);
                }
            }

            // ソケットのクリーンアップ
            closesocket(sock);
            std::cout << "closesocket" << std::endl;
            WSACleanup();
            std::cout << "WSACleanup" << std::endl;
        }
    }
//キー入力があったときのジャンプ箇所
end:
    return 0;
}
