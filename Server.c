#include "Sockets.h"

typedef struct sockaddr_in sockIpv4;

int main()
{
    char *ipPort = "";
    int serverSocketFd = createSocket(IPv4, TCP);
    sockIpv4 *serverAddress = createIPv4Address(ipPort, SERVER_PORT);

    int connectionResult = bind(serverSocketFd, (struct sockaddr *)serverAddress, sizeof(sockIpv4));

    if (connectionResult == 0)
        printf("Server is successfully listening at: %d\n", SERVER_PORT);
    else
    {
        perror("Bind error");
        return 1;
    }

    int listenResult = listen(serverSocketFd, 10);
    printf("Server is now listening... (Waiting for a client to connect)\n");

    startAcceptingIncomingConnections(serverSocketFd);

    shutdown(serverSocketFd, SHUT_RDWR);
}