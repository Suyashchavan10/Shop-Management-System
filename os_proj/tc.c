#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    // ! create a raw socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    printf("Socket created successfully\n");
    printf("Socket id: %d\n", sockfd);

    // create a socket address structure
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        // ! inet_addr val = ip address of the server
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };

    // ! connect to the server
    // ! connect = blocks until a connection is made
    int status = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (status == -1)
    {
        perror("connect");
        return 1;
    }
    printf("Connected to server\n");

    // ! send a message to the server
    
    while(1){
    	char msg[100];
    	printf("Enter a msg to send to server : ");
    	scanf("%[^\n]s", msg);
    	char temp[100];
    	fgets(temp, sizeof(temp), stdin);

   	status = write(sockfd, msg, strlen(msg));
    	if (status == -1)
    	{
    	    perror("write error\n");
    	    return 1;
    	}
    	printf("Message sent to server!\n");

    	char message1[1000];
    	status = read(sockfd, message1, sizeof(message1));
    	if(status == -1){
	    perror("read error\n");
	}

    	printf("Message from server : %s\n", message1);

    }

    close(sockfd);
    return 0;
}