#pragma once

#define STRING_LENGTH 32 //一要素の文字列長
#define NUM_STRINGS 16 //要素数

class WinsockServer
{
public:
    WSADATA wsaData;
    SOCKET listenSock;
    sockaddr_in serverAddr;
    SOCKET clientSock;

    u_short Port;

    bool Protocol; //false=TCP, true=UDP

    char data[NUM_STRINGS][STRING_LENGTH];

    int open();
    int sklisten();
    //int receive();
    //int receive(char* data, size_t size, int flag = 0);
    //int receive(reinterpret_cast<char*> data, size_t size, int flag = 0);
    //int receivef3(float* data, size_t size, int flag = 0); //float[3]を受け取る関数
    int receive_char32x16(char _data[NUM_STRINGS][STRING_LENGTH]);
    int receive();

    int sockend();
    int close();

    float get_float(size_t num); //num番目の要素をfloatで返す
    double get_double(size_t num); //num番目の要素をdoubleで返す
    int get_int(size_t num); //num番目の要素をintで返す

    WinsockServer();
};
