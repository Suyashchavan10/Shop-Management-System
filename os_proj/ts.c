#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

struct product{
	int productId;
	char productName[100];
	int quantity;
	float price;
};

struct cart{
	int userId;
	char userName[100];
	struct product productList[20];
};

int updateQuantity(int fd, int csfd){
	int start = lseek(fd, sizeof(int), SEEK_SET);
	char prodName[100];
	printf("Enter a Product Name to update it's Quantity : ");
	scanf("%[^\n]s", prodName);
	char temp[100];
	fgets(temp, sizeof(temp), stdin);

	struct product prod;
	bool prodFound = false;
	while(read(fd, &prod, sizeof(struct product))){
		if(strcmp(prod.productName, prodName) == 0){
			prodFound = true;
			break;
		}
	}
	if(prodFound){
		struct flock lock;
                lock.l_type = F_WRLCK;
                lock.l_whence = SEEK_CUR;
                lock.l_start = (-1)*(sizeof(struct product));
                lock.l_len = sizeof(struct product);
                lock.l_pid = getpid();
                printf("Inside a critical section of product : %s\n", prodName);
                fcntl(fd, F_SETLKW, &lock);

		int cur = lseek(fd, (-1)*sizeof(struct product), SEEK_CUR);
		int quant = 0;
		printf("Enter Updated Quantity of Product : ");
		scanf("%d", &quant);
		fgets(temp, sizeof(temp), stdin);
		prod.quantity = quant;
		write(fd, &prod, sizeof(struct product));

		printf("End of critical section of product : %s\n", prodName);

                printf("Press Enter to unlock : ");
		fgets(temp, sizeof(temp), stdin);
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                printf("Lock is unlocked!\n");

		printf("Product Quantity updated successfully!\n");
		return 0;
	}
	else{
		printf("Product Name not found\n");
		return -1;
	}

	return 0;
}

int tot = 0;


void *handleClient(void *arg)
{

    int clientSockfd = *(int *)arg;
    int cur = tot;
    	char buffer[100];
	memset(buffer, '\0', sizeof(buffer));
    	int status = read(clientSockfd, buffer, sizeof(buffer));
    	if (status == -1)
    	{
    	    perror("read error\n");
    	    return NULL;
    	}
    	printf("Message from client %d: %s\n", cur, buffer);


	updateQuantity(
	char msg[100];
	printf("Enter a message to send to client %d: ", cur);
	scanf("%[^\n]s", msg);
	char temp[100];
	fgets(temp, sizeof(temp), stdin);

    	status = write(clientSockfd, msg, sizeof(msg));
    	if(status == -1){
	    perror("Write error\n");
	}


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
	tot++;
        status = pthread_create(&thread, NULL, handleClient, &clientSockfd);
        if (status != 0)
        {
            perror("pthread_create");
            return 1;
        }
        pthread_detach(thread);
	//tot--;
       	// ! detach the thread so that it can be cleaned up automatically
    }
    return 0;
}
