# Multi-Threaded TCP Chat Server

A high-performance, raw TCP socket server built in C using the Unix socket library. This project implements a real-time chat architecture where a central server broadcasts messages to multiple concurrent clients.

## Execution Steps

To execute the setup in full, begin by building the socket library, then compile the server and client components while linking them to the generated archive.

**Prerequisites:** Linux/Unix-based OS and the `gcc` compiler.

```bash
# 1. Compile the Sockets library
gcc -c Sockets.c -o Sockets.o
ar rcs Sockets.a Sockets.o

# 2. Build and Link the Server
gcc -c Server.c -o Server.o
gcc -o Server Server.o -L. Sockets.a -lpthread

# 3. Build and Link the Client
gcc -c Client.c -o Client.o
gcc -o Client Client.o -L. Sockets.a -lpthread
```

## Technical Logic & Libraries

### Required Libraries

1. stdio.h: Standard I/O for printing status and messages.
   stdlib.h: Dynamic memory management (e.g., malloc for socket structures).
2. string.h: String manipulation (e.g., strlen for packet sizes and snprintf for message formatting).
3. pthread.h: POSIX threads for handling concurrent client connections.
4. sys/socket.h: Core socket methods and structures.
5. netinet/in.h: Defines IP configuration enums and sockaddr_in.Includes htons (Host to Network Short) to ensure port numbers follow Network Byte Order.
6. arpa/inet.h: Provides inet_pton to convert human-readable IP strings into binary data.

## Component Logic

### 1. Socket Creation (createSocket)

This function abstracts the socket() system call. It takes two parameters:

- IP Type: `AF_INET` (IPv4) or `AF_INET6` (IPv6).
- Socket Type: `SOCK_STREAM` (TCP) or `SOCK_DGRAM` (UDP).

On success, it returns a file descriptor (positive integer); on failure, it returns -1.

### 2. Address Configuration (createIPv4Address)

This builds the sockaddr_in endpoint. It translates the serverPort using htons to ensure it is understood by the network architecture.

```C
struct sockaddr_in *createIPv4Address(char *ip, int serverPort)
{
    struct sockaddr_in *address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    address->sin_port = htons(serverPort);
    address->sin_family = AF_INET;

    if (strlen(ip) == 0)
        address->sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    else
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);

    return address;
}
```

### 3. Server-Side Connection Handling

The server binds to the OS and listens for incoming connections. The **startAcceptingIncomingConnections** function prevents the server from stopping after a single connection:

```C
void startAcceptingIncomingConnections(int serverSocketFd)
{
    while (true)
    {
        ASocket *clientSocket = acceptIncomingConnection(serverSocketFd);
        acceptedSockets[acceptedSocketCount++] = *clientSocket;
        receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
    }
}
```

### 4. Concurrent Data Processing

To prevent the server from blocking while waiting for one user to type, we use pthread_create.

**receiveAndPrintIncomingDataOnSeparateThread**: Spawns the thread and detaches it using pthread_detach to ensure resources are cleaned up by the OS when the client leaves.

**receiveIncomingData**: Runs on the new thread, using recv() to listen for data.

**broadCastMsg**: When data arrives, the server iterates through the acceptedSockets array and sends the data to every client except the original sender.

### 5. Client Broadcast Reception

The client runs a dedicated thread (receiveBroadCastMsgsThread) to listen for broadcasts from the server while the main thread waits for user input via getline. This ensures the client can receive messages even while the user is typing.

## Known Constraints

- Static Array: Currently limited to 10 concurrent connections.

- Race Conditions: Global variables (acceptedSocketCount) are updated without Mutex locks.

- Cleanup: Manual removal of disconnected clients from the array is not yet implemented.

## Credits & Acknowledgments

Special thanks to [Masoudy]('https://github.com/masoudy/CSocket') for the CSocket architecture which served as a foundation for this project's modular design.
