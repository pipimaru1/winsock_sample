#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <cstdio> // snprintf
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

//#define SERVER_IP "10.3.142.146"  // パソコンBのIPアドレスに変更
#define PORT 4410

#define STRING_LENGTH 32
#define NUM_STRINGS 16

void float_to_char32(float x, char str[32]) 
{
    // `snprintf` で float を文字列に変換
    snprintf(str, 32, "%.6f", x); // 小数点以下6桁まで表示
}

//32文字の文字列を16個送る関数
void send_char32x16(SOCKET sock, char data[NUM_STRINGS][STRING_LENGTH]) 
{
    int totalSize = STRING_LENGTH * NUM_STRINGS;
    if (send(sock, reinterpret_cast<char*>(data), totalSize, 0) == SOCKET_ERROR) 
    {
        std::cerr << "Send failed." << std::endl;
    }
}

int main(int argc, char* argv[]) 
{
    // コマンドライン引数のチェック
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <Server IP Address> <polling time[msec]>" << std::endl;
        return 1;
    }    // Winsockの初期化
    else
    {
        std::cout << "Server IP Adress = " << argv[1] << std::endl;
        std::cout << "Polling Time = " << argv[2] << std::endl;
    }
    // サーバーIPアドレスをコマンドライン引数から取得
    const char* serverIP = argv[1];
    int polling_time = atoi(argv[2]);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed" << std::endl;
        return 1;
    }
    else
        std::cout << "WSAStartup success" << std::endl;

    // ソケット作成
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }
    else
        std::cout << "Socket creation success" << std::endl;

    // サーバーアドレスの設定
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);

    // サーバーに接続
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Connection failed" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    else
        std::cout << "Connection success" << std::endl;

    // データの準備
    char data[NUM_STRINGS][STRING_LENGTH] = { "String 1",  "String 2",  "String 3",  "String 4",
                                              "String 5",  "String 6",  "String 7",  "String 8",
                                              "String 9",  "String 10", "String 11", "String 12",
                                              "String 13", "String 14", "String 15", "String 16" };

    // マウス座標の送信ループ
    while (true) {
        POINT mousePos;
        GetCursorPos(&mousePos); // マウスの現在位置を取得
        strcpy_s((char*)data[0], STRING_LENGTH, "CLIENT_001");
        float_to_char32((float)mousePos.x, data[1]);
        float_to_char32((float)mousePos.y, data[2]);
        float_to_char32(0.0f, data[3]);

        send_char32x16(sock, data);

        std::cout << "data[" << NUM_STRINGS<<"]=";
        for (int k = 0; k < NUM_STRINGS; k++)
        {
            std::cout << data[k]<<", ";
        }
        std::cout << std::endl;

        // 座標データを送信
        //int data[2] = { mousePos.x, mousePos.y };
 /*
        float data[3] = { (float)mousePos.x, (float)mousePos.y , 0.0f };
        if (send(sock, (char*)data, sizeof(data), 0) == SOCKET_ERROR) {
            std::cout << "Send failed" << std::endl;
            break;
        }
        else
            std::cout << "Send success: x= " << mousePos.x << " y = " << mousePos.y << std::endl;
*/
        

        Sleep(polling_time); // 50ms待機
    }

    // ソケットのクリーンアップ
    closesocket(sock);
    std::cout << "closesocket" << std::endl;
    WSACleanup();
    std::cout << "WSACleanup" << std::endl;
    return 0;
}
