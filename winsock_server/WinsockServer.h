#pragma once

#define STRING_LENGTH 32 //ˆê—v‘f‚Ì•¶š—ñ’·
#define NUM_STRINGS 16 //—v‘f”

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
    //int receivef3(float* data, size_t size, int flag = 0); //float[3]‚ğó‚¯æ‚éŠÖ”
    int receive_char32x16(char _data[NUM_STRINGS][STRING_LENGTH]);
    int receive();

    int sockend();
    int close();

    float get_float(size_t num); //num”Ô–Ú‚Ì—v‘f‚ğfloat‚Å•Ô‚·
    double get_double(size_t num); //num”Ô–Ú‚Ì—v‘f‚ğdouble‚Å•Ô‚·
    int get_int(size_t num); //num”Ô–Ú‚Ì—v‘f‚ğint‚Å•Ô‚·

    WinsockServer();
};
