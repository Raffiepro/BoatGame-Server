#pragma once

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdint.h>
#include <stdio.h>

static bool startedWsa=false;
static void startWSA() {
    if(!startedWsa) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (result != 0) {
            printf("WSAStartup failed with error: %i",result);
            exit(EXIT_FAILURE);
        }
    }
}

#else

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cstring>
#include <vector>

struct TCPServer {
    std::vector<int> clients;
    int socket;
    sockaddr_in address;
    
    inline TCPServer(uint16_t port) {
        start(port);
    }
    inline TCPServer() {}
    inline ~TCPServer() {
        stop();
    }

    bool setBlocking(bool blocking)
    {
    #ifdef _WIN32
        unsigned long mode = blocking ? 0 : 1;
        return (ioctlsocket(socket, FIONBIO, &mode) == 0);
    #else
        int flags = fcntl(socket, F_GETFL, 0);
        if (flags == -1) return false;
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(socket, F_SETFL, flags) == 0);
    #endif
    }
    void start(uint16_t port) {
        #ifdef _WIN32
        startWSA();
        #endif
        socket = ::socket(AF_INET, SOCK_STREAM, 0);
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = INADDR_ANY;
        bind(socket, (struct sockaddr*)&address,
            sizeof(address));
        listen(socket, 5);
    }
    void accept() {
        int clientSocket = ::accept(socket, nullptr, nullptr);
        clients.push_back(clientSocket);
    }
    inline ssize_t recv(size_t client, void* buff, size_t n) {
        #ifdef _WIN32
        return ::recv(clients[client], (char*)buff, n, 0);
        #else
        return ::recv(clients[client], buff, n, 0);
        #endif
    }
    inline void send(size_t client, const void* buff, size_t n) {
        #ifdef _WIN32
        ::send(clients[client], (const char*)buff, n, 0);
        #else
        ::send(clients[client], buff, n, 0);
        #endif
    }
    inline void stop() {
        #ifdef _WIN32
        closesocket(socket);
        #else
        close(socket);
        #endif
    }
};
struct TCPClient {
    int socket;
    sockaddr_in address;
    
    inline TCPClient(const char* ip, uint16_t port) {
        start(ip, port);
    }
    inline TCPClient() {}
    inline ~TCPClient() {
        stop();
    }

    bool setBlocking(bool blocking)
    {
    #ifdef _WIN32
        unsigned long mode = blocking ? 0 : 1;
        return (ioctlsocket(socket, FIONBIO, &mode) == 0);
    #else
        int flags = fcntl(socket, F_GETFL, 0);
        if (flags == -1) return false;
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(socket, F_SETFL, flags) == 0);
    #endif
    }
    void start(const char* ip, uint16_t port) {
        #ifdef _WIN32
        startWSA();
        #endif
        socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socket < 0) {
            printf("socket error");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0) {
            printf("inet_pton error");
            #ifdef _WIN32
            closesocket(socket);
            #else
            close(socket);
            #endif
            exit(EXIT_FAILURE);
        }
        
        connect(socket, (struct sockaddr*)&address,
        sizeof(address));
    }
    inline ssize_t recv(void* buff, size_t n) const {
        #ifdef _WIN32
        return ::recv(socket, (char*)buff, n, 0);
        #else
        return ::recv(socket, buff, n, 0);
        #endif
    }
    inline void send(const void* buff, size_t n) const {
        #ifdef _WIN32
        ::send(socket, (char*)buff, n, 0);
        #else
        ::send(socket, buff, n, 0);
        #endif
    }
    inline void stop() {
        #ifdef _WIN32
        closesocket(socket);
        #else
        close(socket);
        #endif
    }
};

struct UDPServer {
    int socket;
    sockaddr_in address;
    
    inline UDPServer(uint16_t port) {
        start(port);
    }
    inline UDPServer() {}
    inline ~UDPServer() {
        stop();
    }

    bool setBlocking(bool blocking)
    {
    #ifdef _WIN32
        unsigned long mode = blocking ? 0 : 1;
        return (ioctlsocket(socket, FIONBIO, &mode) == 0);
    #else
        int flags = fcntl(socket, F_GETFL, 0);
        if (flags == -1) return false;
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(socket, F_SETFL, flags) == 0);
    #endif
    }
    void start(uint16_t port) {
        #ifdef _WIN32
        startWSA();
        #endif
        socket = ::socket(AF_INET, SOCK_DGRAM, 0);
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        address.sin_addr.s_addr = INADDR_ANY;
        bind(socket, (struct sockaddr*)&address,
            sizeof(address));
    }
    inline ssize_t recv(sockaddr_in* client, void* buff, size_t n) {
        socklen_t len = sizeof(sockaddr_in);
        #ifdef _WIN32
        return recvfrom(socket,(char*)buff,n,
                0, ( struct sockaddr *) client,
                &len);
        #else
        return recvfrom(socket,(char*)buff,n,
                MSG_WAITALL, ( struct sockaddr *) client,
                &len);
        #endif
    }
    inline void send(sockaddr_in* client, const void* buff, size_t n) {
        #ifdef _WIN32
        sendto(socket, (const char*)buff, n,
            0, (const struct sockaddr *) client,
                sizeof(sockaddr));
        #else
        sendto(socket, (const void *)buff, n,
            MSG_CONFIRM, (const struct sockaddr *) client,
                sizeof(sockaddr));
        #endif
    }
    inline void stop() {
        #ifdef _WIN32
        closesocket(socket);
        #else
        close(socket);
        #endif
    }
};
struct UDPClient {
    int socket;
    sockaddr_in address;
    
    inline UDPClient(const char* ip, uint16_t port) {
        start(ip, port);
    }
    inline UDPClient() {}
    inline ~UDPClient() {
        stop();
    }

    bool setBlocking(bool blocking)
    {
    #ifdef _WIN32
        unsigned long mode = blocking ? 0 : 1;
        return (ioctlsocket(socket, FIONBIO, &mode) == 0);
    #else
        int flags = fcntl(socket, F_GETFL, 0);
        if (flags == -1) return false;
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(socket, F_SETFL, flags) == 0);
    #endif
    }
    void start(const char* ip, uint16_t port) {
        #ifdef _WIN32
        startWSA();
        #endif
        socket = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (socket < 0) {
            printf("socket error");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &address.sin_addr) <= 0) {
            printf("inet_pton error");
            #ifdef _WIN32
            closesocket(socket);
            #else
            close(socket);
            #endif
            exit(EXIT_FAILURE);
        }
    }
    inline ssize_t recv(void* buff, size_t n) const {
        socklen_t len = sizeof(address);
        #ifdef _WIN32
        return recvfrom(socket, (char*)buff, n, 
				0, (struct sockaddr *) &address, 
				&len);
        #else
        return recvfrom(socket, buff, n, 
				MSG_WAITALL, (struct sockaddr *) &address, 
				&len);
        #endif
    }
    inline void send(const void* buff, size_t n) const {
        #ifdef _WIN32
        sendto(socket, (const char*)buff, n, 
		0, (const struct sockaddr *) &address, 
			sizeof(address));
        #else
        sendto(socket, buff, n, 
		MSG_CONFIRM, (const struct sockaddr *) &address, 
			sizeof(address));
        #endif
    }
    inline void stop() {
        #ifdef _WIN32
        closesocket(socket);
        #else
        close(socket);
        #endif
    }
};
