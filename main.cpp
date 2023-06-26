#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include<iostream>
#include<vector>
#include<unordered_map>
static int MAX_CLIENTLINK = 3;



// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

int TcpServer() {
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
    SOCKET ListenSocket;
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound.
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(12345);
    inet_pton(AF_INET, "10.77.24.242", &service.sin_addr);

    if (bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    //----------------------
    // Listen for incoming connection requests.
    // on the created socket
    if (listen(ListenSocket, 10) == SOCKET_ERROR) {
        wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    //----------------------
    // Create a SOCKET for accepting incoming requests.
    SOCKET AcceptSocket;
    wprintf(L"Waiting for client to connect...\n");

    sockaddr_in acceptaddr;
    acceptaddr.sin_family = AF_INET;
    int addrlen = sizeof(acceptaddr);
    AcceptSocket = accept(ListenSocket, (sockaddr*)&acceptaddr, &addrlen);

    if (AcceptSocket == INVALID_SOCKET) {
        wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    else
        wprintf(L"Client connected.\n");


    char recvStr[1024] = {};
    int len = sizeof(recvStr);
    int count = 0;
    while (1) {

        memset(recvStr, 0, len);
        if (recv(AcceptSocket, recvStr, len, 0) < 0) {
            std::cout << "�ͻ��˶Ͽ�" << std::endl;
            return 1;
        }
        else {

            int port = ntohs(acceptaddr.sin_port);
            char host[128] = {};
            inet_ntop(AF_INET, &acceptaddr.sin_addr, host, 128);
            count++;
            std::cout << "�������� ��ַ��" << host << "  �˿ڣ�" << port << "  ��Ϣ��" << recvStr << "       ��������" << count << "������" << std::endl;
            //Sleep(10);
        }
    }

    // No longer need server socket
    closesocket(ListenSocket);

    WSACleanup();
    return 0;
}

int SelectMode() {

    std::unordered_map<SOCKET, sockaddr_in> clientaddr_map;  //���ӷ������Ŀͻ��˵�ַ����

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
    SOCKET ListenSocket;
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound.
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(12345);
    inet_pton(AF_INET, "10.77.24.242", &service.sin_addr);

    if (bind(ListenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    //----------------------
    // Listen for incoming connection requests.
    // on the created socket
    if (listen(ListenSocket, 3) == SOCKET_ERROR) {
        wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    fd_set listenfds, clientfds, readfds, writefds;
    FD_ZERO(&listenfds);
    FD_ZERO(&clientfds);
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    FD_SET(ListenSocket, &listenfds);
    TIMEVAL time = { 0,0 };

    while (1) {

        readfds = listenfds;
        int ret = select(0, &readfds, NULL, NULL, &time);

        //listenfd�ɶ����ͻ��˷�������������listensocket�ɶ���
        if (ret > 0) {
            if (FD_ISSET(ListenSocket, &readfds)) {
                if (clientfds.fd_count < MAX_CLIENTLINK) {

                    SOCKET AcceptSocket;
                    sockaddr_in acceptaddr;
                    acceptaddr.sin_family = AF_INET;
                    int addrlen = sizeof(acceptaddr);
                    AcceptSocket = accept(ListenSocket, (sockaddr*)&acceptaddr, &addrlen);

                    if (AcceptSocket == INVALID_SOCKET) {
                        wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
                        continue;
                    }
                    else {
                        FD_SET(AcceptSocket, &clientfds);

                        clientaddr_map[AcceptSocket] = acceptaddr;  //���˿ͻ���ip�Ͷ˿ںŴ��ڹ�ϣmap��
                        //��ӡ�µ�������Ϣ
                        int port = ntohs(acceptaddr.sin_port);
                        char host[128] = {};
                        inet_ntop(AF_INET, &acceptaddr.sin_addr, host, 128);
                        std::cout << "������һ�������� ��ַ��" << host << "  �˿ڣ�" << port << "  ����" << clientfds.fd_count << "������" << std::endl;
                        
                        char sendStr[24] = {"accept connection"};

                        int len = sizeof(sendStr);
                        int iResult = send(AcceptSocket, sendStr, len, 0);
                        if (iResult == SOCKET_ERROR) {
                            wprintf(L"send failed with error: %d\n", WSAGetLastError());
                            closesocket(AcceptSocket);
                            std::cout << "����ʧ�ܣ�" << std::endl;
                        }

                    }
                }
                else {
                    //std::cout << "���ӵĿͻ�������" << std::endl;
                }
            }
            
        }


        writefds = clientfds;
        int retc = select(0, &writefds, NULL, NULL, &time);
        //��clientfdsû����Ӧ��û�н�������
        if (retc <= 0) {
            continue;
        }
        
        //���ͻ���������д��
        for (int i = 0; i < clientfds.fd_count; i++) {
            
            if (FD_ISSET(clientfds.fd_array[i], &writefds)) {
                char buf[1024];
                int ret_recv = recv(clientfds.fd_array[i], buf, sizeof(buf), 0);

                if (ret_recv == -1) {

                    int port = ntohs(clientaddr_map[clientfds.fd_array[i]].sin_port);
                    char host[128] = {};
                    inet_ntop(AF_INET, &clientaddr_map[clientfds.fd_array[i]].sin_addr, host, 128);

                    std::cout << "�ͻ����ѶϿ�����   " << "��ַ��" << host << " �˿ںţ�" << port << " ";

                    //�ӿͻ��˼�����ɾ����ǰ�Ͽ��Ŀͻ���
                    FD_CLR(clientfds.fd_array[i], &clientfds);
                    std::cout << "ʣ��������" << clientfds.fd_count << std::endl;
                }
                else if (ret_recv == 0) {
                    int port = ntohs(clientaddr_map[clientfds.fd_array[i]].sin_port);
                    char host[128] = {};
                    inet_ntop(AF_INET, &clientaddr_map[clientfds.fd_array[i]].sin_addr, host, 128);

                    std::cout << "�ͻ����ѶϿ�����   " << "��ַ��" << host << " �˿ںţ�" << port << " ";
                    std::cout << "�������� ��ַ��" << host << " �˿ڣ�" << port << " ����Ϣʧ��" ;

                    FD_CLR(clientfds.fd_array[i], &clientfds);
                    std::cout << "ʣ��������" << clientfds.fd_count << std::endl;
                }
                else {
                    int port = ntohs(clientaddr_map[clientfds.fd_array[i]].sin_port);
                    char host[128] = {};
                    inet_ntop(AF_INET, &clientaddr_map[clientfds.fd_array[i]].sin_addr, host, 128);
                    std::cout << "�������� ��ַ��" << host << "  �˿ڣ�" << port << "  ��Ϣ��" << buf << std::endl;

                }
            }
        }
     }

        

    WSACleanup();
    return 0;
 } 

int main()
{
    SelectMode();
   
}