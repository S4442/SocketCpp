#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include<iostream>
#include<string>
using namespace std;

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

int main()
{

    //----------------------
    // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error: %ld\n", iResult);
        return 1;
    }
    //----------------------
    // Create a SOCKET for listening for
    // incoming connection requests.
    SOCKET ConnectSocket;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound.
    sockaddr_in csockaddr;
    csockaddr.sin_family = AF_INET;
    csockaddr.sin_port = htons(12345);
    inet_pton(AF_INET, "10.77.24.242", &csockaddr.sin_addr);
    //inet_pton(AF_INET, "10.77.24.242", &csockaddr.sin_addr);


    fd_set sockfds;
    FD_ZERO(&sockfds);
    FD_SET(ConnectSocket, &sockfds);

    iResult = connect(ConnectSocket, (SOCKADDR*)&csockaddr, sizeof(csockaddr));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
        iResult = closesocket(ConnectSocket);
        if (iResult == SOCKET_ERROR)
            wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    timeval time = { 5,0 };

    int ret_recv = select(1, &sockfds, NULL, NULL, &time);
    int ret = -1;

    if (ret_recv <= 0) {
        std::cout << "等待服务器响应" << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    else {

        char buf[24];
        int ret_recv = recv(ConnectSocket, buf, sizeof(buf), 0);
        if ((strcmp(buf, "accept connection") == 0)) {
            std::cout << "Connected to server." << std::endl;
            ret = 8080;
        }
        else
            std::cout << "连接发生错误，服务器响应：" << buf << std::endl;
    }



    while (ret==8080) {
        char sendStr[1024] = {};
        cout << "请输入要发送的消息：";
        cin >> sendStr;

        int len = sizeof(sendStr);
        iResult = send(ConnectSocket, sendStr, len, 0);
        if (iResult == SOCKET_ERROR) {
            wprintf(L"send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            cout << "发送失败！" << endl;
        }
        else {
            cout << "发送：" << sendStr << endl;
        }
    }


    // No longer need server socket
    /*iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }*/

    WSACleanup();
    return 0;
}