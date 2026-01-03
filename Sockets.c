#include "Sockets.h"

ASocket acceptedSockets[10];
static int acceptedSocketCount = 0;

int createSocket(IpType ipType, Stype socketType)
{
    int ip = ipType == IPv4 ? AF_INET : AF_INET6,
        tcpType = socketType == TCP ? SOCK_STREAM : SOCK_DGRAM;

    return socket(ip, tcpType, 0);
}

struct sockaddr_in *createIPv4Address(char *ip, int serverPort)
{
    struct sockaddr_in *address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

    address->sin_port = htons(serverPort);
    address->sin_family = AF_INET;

    if (strlen(ip) == 0)
        address->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);

    return address;
}

ASocket *acceptIncomingConnection(int serverSocketFd)
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddrSize = sizeof(struct sockaddr_in);
    int clientSocketFd = accept(serverSocketFd, (struct sockaddr *)&clientAddress, &clientAddrSize);

    ASocket *result = (ASocket *)malloc(sizeof(ASocket));
    result->address = clientAddress;
    result->acceptSocketFd = clientSocketFd;
    result->acceptedSuccessfully = clientSocketFd > 0;

    if (!result->acceptedSuccessfully)
        result->error = clientSocketFd;

    return result;
}

void broadCastMsg(char *buffer, int socketFd)
{
    for (int i = 0; i < acceptedSocketCount; i++)
    {
        int targetFd = acceptedSockets[i].acceptSocketFd;

        if (targetFd > 0 && targetFd != socketFd)
            send(targetFd, buffer, strlen(buffer), 0);
        }
}

void *listenAndPrintMsgsOnNewThread(void *socketFd)
{
    int socketFD = (int)(intptr_t)socketFd;
    char buffer[1024];

    while (true)
    {
        ssize_t amoutReceived = recv(socketFD, buffer, 1024, 0);

        if (amoutReceived > 0)
        {
            buffer[amoutReceived] = 0;
            printf("%s", strlen(buffer) == 0 ? "NULL" : buffer);

            broadCastMsg(buffer, socketFD);
        }

        if (amoutReceived <= 0)
            break;
    }

    close(socketFD);

    return NULL;
}

void receiveBroadCastMsgsThread(int socketFd)
{
    pthread_t id;
    pthread_create(&id, NULL, listenAndPrintMsgsOnNewThread, (void *)(intptr_t)socketFd);
    pthread_detach(id); // Good practice to detach if you aren't joining
}

void *receiveIncomingData(void *socketFd)
{
    int socketFD = (int)(intptr_t)socketFd;
    char buffer[1024];

    while (true)
    {
        ssize_t amoutReceived = recv(socketFD, buffer, 1024, 0);

        if (amoutReceived > 0)
        {
            buffer[amoutReceived] = 0;
            printf("%s", strlen(buffer) == 0 ? "NULL" : buffer);

            broadCastMsg(buffer, socketFD);
        }

        if (amoutReceived <= 0)
            break;
    }

    close(socketFD);
    return NULL;
}

void receiveAndPrintIncomingDataOnSeparateThread(ASocket *threadClientSocket)
{
    pthread_t id;

    pthread_create(&id, NULL, receiveIncomingData, (void *)(intptr_t)threadClientSocket->acceptSocketFd);

    pthread_detach(id);
}

void startAcceptingIncomingConnections(int serverSocketFd)
{

    while (true)
    {
        ASocket *clientSocket = acceptIncomingConnection(serverSocketFd);
        acceptedSockets[acceptedSocketCount++] = *clientSocket;
        receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
    }
}
