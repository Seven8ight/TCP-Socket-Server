#include "Sockets.h"

int main()
{
    int socketFD = createSocket(IPv4, TCP);

    if (socketFD < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }

    struct sockaddr_in *address = createIPv4Address(IPV4, SERVER_PORT);

    // Handshake connection here - TCP-3 Way handshake
    int result = connect(socketFD, (const struct sockaddr *)address, sizeof(struct sockaddr_in));

    if (result == 0)
        printf("Connection successful\n");
    else
        printf("Connecion failed\n");

    char *name = NULL;
    size_t nameSize = 0;
    printf("Enter your name: ");
    ssize_t nameCount = getline(&name, &nameSize, stdin);
    name[nameCount - 1] = 0;

    char *line = NULL;
    size_t lineSize = 0;
    printf("Type in a message to send\n");

    receiveBroadCastMsgsThread(socketFD);

    char buffer[1024];

    while (true)
    {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        line[lineSize - 1] = 0;

        snprintf(buffer, sizeof(buffer), "%s: %s", name, line);

        if (charCount > 0)
        {
            if (strcmp(line, "exit") == 0)
                break;
            ssize_t sendCount = send(socketFD, buffer, strlen(buffer), 0);
        }
    }

    close(socketFD);
    // Note the forward slash / and the space after Host:
    // char message[] = "GET / HTTP/1.1\r\nHost: neverssl.com\r\nConnection: close\r\n\r\n";

    // char buffer[1024];
    // recv(socketFD, buffer, 1024, 0);

    // printf("Response was %s\n", strlen(buffer) == 0 ? "NULL" : buffer);
}

/*
    Socket function takes 3 parameters:
        1. Type of ip address to handle either ipv4 or 6 described as
            AF_INET - ipv4
            AF_INET6 - ipv6
        2. Type of socket to use either UDP or TCP described as
            SOCK_STREAM - TCP
            SOCK_DGRAM - UDP
        3. Protocol - layer beneath the transport socket we use.
            0 - ip layer to be used underneath the transport layer

    The socket function returns a positive integer on success or -1 on failure

    Connect function takes 3 params:
        1. socket descriptor - the created socket prior to connection
        2. connection address
*/