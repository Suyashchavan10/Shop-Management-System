// client for user and admin
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


    // !send a message
    char message[] = "";
    int auth_type, client_type;
    printf("Enter 1 for LOGIN and 2 for SIGNIN : ");
    scanf("%d", &auth_type);

    if(auth_type == 1){
	char auth_type1[] = "LOGIN ";
	strcat(message, auth_type1);
	printf("Client entered in LOGIN page\n");
	printf("Enter 1 to LOGIN as USER and 2 to LOGIN as ADMIN : ");
	scanf("%d", &client_type); 
	if(client_type == 1){
		char client_type1[] = "USER ";
		strcat(message, client_type1);

		printf("Successful LOGIN as USER\n");
	}
	else if(client_type == 2){
		char client_type2[] = "ADMIN ";
		strcat(message, client_type2);
		printf("Successful LOGIN as ADMIN\n");
	}
	else{
		printf("Wrong choice!\n");
	}
    }
    else if(auth_type == 2){
	char auth_type2[] = "SIGNIN ";
	strcat(message, auth_type2);
    	printf("Client entered in SIGNIN page\n");
	printf("Enter 1 to SIGNIN as USER and 2 to SIGNIN as ADMIN : ");
	scanf("%d", &client_type); 
	if(client_type == 1){
		char client_type1[] = "USER ";
		strcat(message, client_type1);


		printf("Successful SIGNIN as USER\n");
	}
	else if(client_type == 2){
		char client_type2[] = "ADMIN ";
		strcat(message, client_type2);

		printf("Successful SIGNIN as ADMIN\n");
	}
	else{
		printf("Wrong choice!\n");
	}
    }
    else{
    	printf("Wrong choice!\n");
    }


    printf("%s", message);    
    status = write(sockfd, message, strlen(message));
    if (status == -1)
    {
       	perror("write");
       	return 1;
    }
    printf("Message sent to server\n");
    close(sockfd);
    return 0;
}
