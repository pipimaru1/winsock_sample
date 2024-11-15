#pragma once

#define STRING_LENGTH 32
#define NUM_STRINGS 16

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
    int recievef3(float* data, size_t size, int flag = 0); //float[3]‚ğó‚¯æ‚éŠÖ”
    int recieve_char32x16(char data[NUM_STRINGS][STRING_LENGTH]);

    int sockend();
    int close();

    WinsockServer();
};
