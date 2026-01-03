#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define IPV4 "127.0.0.1"

typedef enum iPAddrType
{
    IPv4,
    IPv6
} IpType;

typedef enum SocketType
{
    TCP,
    UDP
} Stype;

typedef struct acceptedSocket
{
    int acceptSocketFd;
    int error;
    bool acceptedSuccessfully;
    struct sockaddr_in address;

} ASocket;

int createSocket(IpType ipType, Stype socketType);

struct sockaddr_in *createIPv4Address(char *ip, int serverPort);

ASocket *acceptIncomingConnection(int serverSocketFd);

void *receiveIncomingData(void *socketFd);

void startAcceptingIncomingConnections(int serverSocketFd);

void receiveAndPrintIncomingDataOnSeparateThread(ASocket *threadClientSocketFd);

void broadCastMsg(char *buffer, int socketFd);

void receiveBroadCastMsgsThread(int socketFd);

void *listenAndPrintMsgsOnNewThread(void *socketFd);
