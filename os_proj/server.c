// concurrent server using threads for each client

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void *handleClient(void *arg)
{
    int clientSockfd = *(int *)arg;
    char buffer[1024];
    int status = read(clientSockfd, buffer, sizeof(buffer));
    if (status == -1)
    {
        perror("read");
        return NULL;
    }
    printf("Message from client: %s\n", buffer);
    close(clientSockfd);
    return NULL;
}

int main()
{
    // ! create a raw socket
    // socket(int domain, int type, int protocol)
    // AF_INET - family of protocols to be used
    // SOCK_STREAM - type of socket = TCP
    // UDP = SOCK_DGRAM
    // 0 - protocol - 0 for default
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }
    printf("Socket created successfully\n");

    // ! create a socket address structure
    struct sockaddr_in serverAddr = {
        .sin_addr.s_addr = INADDR_ANY, // ! INADDR_ANY = allow any address on the machine
        .sin_family = AF_INET,         // ! AF_INET = IPv4
        .sin_port = htons(8080)        // ! htons = host to network short - since we are using intel architecture
    };

    // ! bind the socket to the address
    int status = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (status == -1)
    {
        perror("bind");
        return 1;
    }
    printf("Socket binded successfully\n");

    // ! listen for connections
    int backlog = 5;
    status = listen(sockfd, backlog); // ! backlog = max number of pending connections in the queue

    while (1)
    {
        // ! accept a connection
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        // ! accept = blocks until a connection is made
        printf("Listening for connections on port 8080\n");
        int clientSockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSockfd == -1)
        {
            perror("accept");
            return 1;
        }
        printf("Connection accepted\n");

        // ! create a thread to handle the request
        pthread_t thread;
        status = pthread_create(&thread, NULL, handleClient, &clientSockfd);
        if (status != 0)
        {
            perror("pthread_create");
            return 1;
        }
        pthread_detach(thread); // ! detach the thread so that it can be cleaned up automatically
    }
    return 0;
}
