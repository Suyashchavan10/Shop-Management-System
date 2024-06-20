// ! client for q34_a.c
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

    //signin user - 2 1 username password
    //signin admin - 2 2 username password
    //login user - 1 1 username password
    //login admin - 1 2 username password

    
    char message[100] = "";
    char temp[100];
    char userName[100];
    int auth_type, client_type;

    while(1){
    memset(message, '\0', sizeof(message));
    printf("Enter 1 for LOGIN ,2 for SIGNIN, 3 for Exit : ");
    scanf("%d", &auth_type);
	fgets(temp, sizeof(temp), stdin);

    if(auth_type == 1){
	char auth_type1[] = "1-";
	strcat(message, auth_type1);
	printf("Client entered in LOGIN page\n");
	printf("Enter 1 to LOGIN as USER and 2 to LOGIN as ADMIN : ");
	scanf("%d", &client_type); 
	fgets(temp, sizeof(temp), stdin);
	if(client_type == 1){
		char client_type1[] = "1-";
		strcat(message, client_type1);
		char name[100];
		printf("Enter Name : ");
		scanf("%[^\n]s", name);
		fgets(temp, sizeof(temp), stdin);
		strcat(message, name);
		strcat(message, "-");
		strcpy(userName, name);
		
		char pass[100];
		printf("Enter Password : ");
		scanf("%[^\n]s", pass);
                fgets(temp, sizeof(temp), stdin);
		strcat(message, pass);
		//strcat(message, "-");

		//printf("Successful LOGIN as USER\n");
	}
	else if(client_type == 2){
		char client_type2[] = "2-";
		strcat(message, client_type2);
		char name[100];
                printf("Enter Name : ");
                scanf("%[^\n]s", name);
                fgets(temp, sizeof(temp), stdin);
                strcat(message, name);
		strcat(message, "-");
		strcpy(userName, name);

                char pass[100];
                printf("Enter Password : ");
                scanf("%[^\n]s", pass);
                fgets(temp, sizeof(temp), stdin);
                strcat(message, pass);
		//strcat(message, "-");

		//printf("Successful LOGIN as ADMIN\n");
	}
	else{
		//printf("Wrong choice!\n");
	}
    }
    else if(auth_type == 2){
	char auth_type2[] = "2-";
	strcat(message, auth_type2);
    	printf("Client entered in SIGNIN page\n");
	printf("Enter 1 to SIGNIN as USER and 2 to SIGNIN as ADMIN : ");
	scanf("%d", &client_type); 
	fgets(temp, sizeof(temp), stdin);
	if(client_type == 1){
		char client_type1[] = "1-";
		strcat(message, client_type1);
		char name[100];
                printf("Enter Name : ");
                scanf("%[^\n]s", name);
                fgets(temp, sizeof(temp), stdin);
                strcat(message, name);
		strcat(message, "-");
		strcpy(userName, name);

                char pass[100];
                printf("Enter Password : ");
                scanf("%[^\n]s", pass);
                fgets(temp, sizeof(temp), stdin);
                strcat(message, pass);
		//strcat(message, "-");

		//printf("Successful SIGNIN as USER\n");
	}
	else if(client_type == 2){
		char client_type2[] = "2-";
		strcat(message, client_type2);
		char name[100];
                printf("Enter Name : ");
                scanf("%[^\n]s", name);
                fgets(temp, sizeof(temp), stdin);
                strcat(message, name);
		strcat(message, "-");
		strcpy(userName, name);

                char pass[100];
                printf("Enter Password : ");
                scanf("%[^\n]s", pass);
                fgets(temp, sizeof(temp), stdin);
                strcat(message, pass);
		//strcat(message, "-");

		//printf("Successful SIGNIN as ADMIN\n");
	}
	else{
		//printf("Wrong choice!\n");
	}
    }
    else{
    	//printf("Wrong choice!\n");
    }


    printf("%s\n", message);

    printf("Hit enter to send a message : ");
    fgets(temp, 100, stdin);

    //char message[1000] = "Hi server";
    status = write(sockfd, message, strlen(message));
    if (status == -1)
    {
        perror("write error\n");
        return 1;
    }
    printf("Message sent to server\n");

    char message1[1000];
    memset(message1, '\0', sizeof(message1));
    status = read(sockfd, message1, sizeof(message1));
    if(status == -1){
	    perror("read error\n");
    }

    printf("message from server : %s\n", message1);

    if(message[0] == '1'){
	    //LOGIN
	if(message[2] == '1'){
		if(strcmp(message1, "0") == 0){
			printf("LOGIN as USER SUCCESSFUL!\n");
		}
		else if(strcmp(message1, "-1") == 0){
			printf("LOGIN as USER UNSUCCESSFUL!\n");
		}
	}
	else if(message[2] == '2'){
		if(strcmp(message1, "0") == 0){
                        printf("LOGIN as ADMIN SUCCESSFUL!\n");
                }
                else if(strcmp(message1, "-1") == 0){
                        printf("LOGIN as ADMIN UNSUCCESSFUL!\n");                }
	}	
	break;
    }
    else if(message[0] == '2'){
	    //SIGNIN
	if(message[2] == '1'){
		if(strcmp(message1, "0") == 0){
                        printf("SIGNIN as USER SUCCESSFUL!\n");
                }
                else if(strcmp(message1, "-1") == 0){
                        printf("SIGNIN as USER UNSUCCESSFUL!\n");                }
        }
        else if(message[2] == '2'){
		if(strcmp(message1, "0") == 0){
                        printf("SIGNIN as ADMIN SUCCESSFUL!\n");
                }
                else if(strcmp(message1, "-1") == 0){
                        printf("SIGNIN as ADMIN UNSUCCESSFUL!\n");                }
        }
    }
    else{
	    printf("Wrong Choice!\n");
    }
    
    }
    //LOGIN and SIGNIN ends here



    //main methods starts here
    if(message[2] == '1'){
	//USER
	printf("********************** USER METHODS ****************\n");
	printf("->Following are User Methods with method types : \n");
	printf("3 - Display Products\n");
	printf("4 - Display Cart\n");
	printf("5 - Add Product to Cart\n");
	printf("6 - Edit Cart\n");
	printf("7 - Payment\n");
	printf("12 - Exit\n");

	while(1){
		int type = 0;
		char message3[100] = "";
    		char temp1[100];
		printf("Enter a method type : ");
		scanf("%d", &type);
		fgets(temp1, sizeof(temp1), stdin);

		if(type == 3){
			//3 - Display Products
			char stype[] = "3";
			strcat(message3, stype);
                	//strcat(message, "-");

			printf("%s\n", message3);

        		printf("Hit enter to send a message : ");
        		fgets(temp1, 100, stdin);

        		//char message[1000] = "Hi server";
        		status = write(sockfd, message3, strlen(message3));
        		if (status == -1)
        		{
            			perror("write error\n");
            			return 1;
        		}
        		printf("Message sent to server\n");
		}
		else if(type == 4){
			//4 - Display Cart
			char stype[] = "4-";
                        strcat(message3, stype);
			strcat(message3, userName);

			printf("%s\n", message3);

                        printf("Hit enter to send a message : ");
                        fgets(temp1, 100, stdin);

                        //char message[1000] = "Hi server";
                        status = write(sockfd, message3, strlen(message3));
                        if (status == -1)
                        {
                                perror("write error\n");
                                return 1;
                        }
                        printf("Message sent to server\n");
		}
		else if(type == 5){
			//5 - Add Product to Cart
			char stype[] = "5-";
                        strcat(message3, stype);
			strcat(message3, userName);
			strcat(message3, "-");
			char productName[100];
                	printf("Enter a product name to add to cart : ");
                	scanf("%[^\n]s", productName);
                	fgets(temp, sizeof(temp), stdin);

                	char quant[10];
                	printf("Enter a Quantity of Product to buy : ");
                	scanf("%[^\n]s", quant);
                	fgets(temp, sizeof(temp), stdin);
			strcat(message3, productName);
			strcat(message3, "-");
			strcat(message3, quant);

			printf("%s\n", message3);

                        printf("Hit enter to send a message : ");
                        fgets(temp1, 100, stdin);

                        //char message[1000] = "Hi server";
                        status = write(sockfd, message3, strlen(message3));
                        if (status == -1)
                        {
                                perror("write error\n");
                                return 1;
                        }
                        printf("Message sent to server\n");
			
		}
		else if(type == 6){
			//6 - Edit Cart
			char stype[] = "6-";
                        strcat(message3, stype);
                        strcat(message3, userName);
                        strcat(message3, "-");
                        char productName[100];
                        printf("Enter a product name to edit its Quantity in the cart : ");
                        scanf("%[^\n]s", productName);
                        fgets(temp, sizeof(temp), stdin);

                        char quant[10];
                        printf("Enter a New Quantity of Product : ");
                        scanf("%[^\n]s", quant);
                        fgets(temp, sizeof(temp), stdin);
                        strcat(message3, productName);
                        strcat(message3, "-");
                        strcat(message3, quant);

			printf("%s\n", message3);

                        printf("Hit enter to send a message : ");
                        fgets(temp1, 100, stdin);

                        //char message[1000] = "Hi server";
                        status = write(sockfd, message3, strlen(message3));
                        if (status == -1)
                        {
                                perror("write error\n");
                                return 1;
                        }
                        printf("Message sent to server\n");
		}
		else if(type == 7){
			//7 - Payment
			char stype[] = "7-";
                        strcat(message3, stype);
			strcat(message3, userName);

			printf("%s\n", message3);

                        printf("Hit enter to send a message : ");
                        fgets(temp1, 100, stdin);

                        //char message[1000] = "Hi server";
                        status = write(sockfd, message3, strlen(message3));
                        if (status == -1)
                        {
                                perror("write error\n");
                                return 1;
                        }
                        printf("Message sent to server\n");

			char message2[1000];
                        memset(message2, '\0', sizeof(message2));

                        status = read(sockfd, message2, sizeof(message2));
                        if(status == -1){
                                perror("read error\n");
                        }

                        printf("message from server : %s\n", message2);
			char amtPaid[20];
			scanf("%s", amtPaid);
			status = write(sockfd, amtPaid, strlen(amtPaid));
                        if (status == -1)
                        {
                                perror("write error\n");
                                return 1;
                        }
                        printf("Message sent to server\n");

			memset(message2, '\0', sizeof(message2));

                        status = read(sockfd, message2, sizeof(message2));
                        if(status == -1){
                                perror("read error\n");
                        }

                        printf("message from server : %s\n", message2);
			fgets(temp, 100, stdin);
			fgets(temp, 100, stdin);
			char message4[1000] = "Unlock All the locks";
                        status = write(sockfd, message3, strlen(message3));
                        if (status == -1)
                        {
                                perror("write error\n");
                                return 1;
                        }
                        printf("Message sent to server\n");

		}
		else if(type == 12){
			//12 - Exit
			//char stype[] = "12";
                        //strcat(message3, stype);
			printf("************************* You are successfully Exited! ***********************\n");
			return 0;
		}
		else{
			printf("Wrong Choice!\n");
		}

	/*
    	printf("%s\n", message3);

    	printf("Hit enter to send a message : ");
    	fgets(temp1, 100, stdin);

    	//char message[1000] = "Hi server";
    	status = write(sockfd, message3, strlen(message3));
    	if (status == -1)
    	{
    	    perror("write error\n");
       	    return 1;
    	}
    	printf("Message sent to server\n");
	*/

    	char message2[2000];
    	memset(message2, '\0', sizeof(message2));

    	status = read(sockfd, message2, sizeof(message2));
    	if(status == -1){
	    perror("read error\n");
    	}
	
    	printf("message from server : %s\n", message2);

    	}
	
    }


    else if(message[2] == '2'){
	//ADMIN
	printf("********************** ADMIN METHODS ****************\n");
        printf("->Following are ADMIN Methods with method types : \n");
	printf("8 - Add Product\n");
        printf("9 - Delete Product\n");
        printf("10 - Update Quantity\n");
        printf("11 - Update Price\n");
        printf("12 - Exit\n");

        while(1){
		int type = 0;
		char message3[100] = "";
    		char temp1[100];
		printf("Enter a method type : ");
		scanf("%d", &type);
		fgets(temp1, sizeof(temp1), stdin);

		if(type == 8){
			//8 - Add Product
			char stype[] = "8-";
			strcat(message3, stype);
			char productName[100];
                        printf("Enter a Product Name to add : ");
                        scanf("%[^\n]s", productName);
                        fgets(temp, sizeof(temp), stdin);

                        char quant[10];
                        printf("Enter Quantity of Product : ");
                        scanf("%[^\n]s", quant);
                        fgets(temp, sizeof(temp), stdin);

			char price[10];
			printf("Enter Price of Product : ");
			scanf("%[^\n]s", price);
                        fgets(temp, sizeof(temp), stdin);

                        strcat(message3, productName);
                        strcat(message3, "-");
                        strcat(message3, quant);
			strcat(message3, "-");
                	strcat(message3, price);

			printf("%s\n", message3);

    			printf("Hit enter to send a message : ");
    			fgets(temp1, 100, stdin);

    			//char message[1000] = "Hi server";
    			status = write(sockfd, message3, strlen(message3));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}
    			printf("Message sent to server\n");

    			char message2[1000];
    			memset(message2, '\0', sizeof(message2));

    			status = read(sockfd, message2, sizeof(message2));
    			if(status == -1){
            			perror("read error\n");
    			}

    			printf("message from server : %s\n", message2);

    			printf("Hit enter to continue in critical section : ");
    			fgets(temp1, 100, stdin);

    			char message4[1000] = "Continue Critical section";
    			status = write(sockfd, message4, strlen(message4));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}

    			memset(message2, '\0', sizeof(message2));

    			status = read(sockfd, message2, sizeof(message2));
    			if(status == -1){
            			perror("read error\n");
    			}

    			printf("message from server : %s\n", message2);

			//printf("Hit enter to continue in critical section : ");
    			fgets(temp1, 100, stdin);

    			char message5[1000] = "UnLock the Lock";
    			status = write(sockfd, message5, strlen(message5));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}

		}
		else if(type == 9){
			//9 - Delete Product
			char stype[] = "9-";
                      	strcat(message3, stype);
			char productName[100];
                        printf("Enter a Product Name to delete : ");
                        scanf("%[^\n]s", productName);
                        fgets(temp, sizeof(temp), stdin);
			strcat(message3, productName);

			printf("%s\n", message3);

    			printf("Hit enter to send a message : ");
    			fgets(temp1, 100, stdin);

    			//char message[1000] = "Hi server";
    			status = write(sockfd, message3, strlen(message3));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}
    			printf("Message sent to server\n");

    			char message2[1000];
    			memset(message2, '\0', sizeof(message2));

    			status = read(sockfd, message2, sizeof(message2));
    			if(status == -1){
            			perror("read error\n");
    			}

    			printf("message from server : %s\n", message2);

    			printf("Hit enter to continue in critical section : ");
    			fgets(temp1, 100, stdin);

    			char message4[1000] = "Continue Critical section";
    			status = write(sockfd, message4, strlen(message4));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}

    			memset(message2, '\0', sizeof(message2));

    			status = read(sockfd, message2, sizeof(message2));
    			if(status == -1){
            			perror("read error\n");
    			}

    			printf("message from server : %s\n", message2);

			//printf("Hit enter to continue in critical section : ");
    			fgets(temp1, 100, stdin);

    			char message5[1000] = "UnLock the Lock";
    			status = write(sockfd, message5, strlen(message5));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}

		}
		else if(type == 10){
			//10 - Update Quantity
			char stype[] = "10-";
                        strcat(message3, stype);
			char productName[100];
                	printf("Enter a Product Name to update it's Quantity : ");
                	scanf("%[^\n]s", productName);
                	fgets(temp, sizeof(temp), stdin);

                	char quant[10];
                	printf("Enter Updated Quantity of Product : ");
                	scanf("%[^\n]s", quant);
                	fgets(temp, sizeof(temp), stdin);
			strcat(message3, productName);
			strcat(message3, "-");
			strcat(message3, quant);

			printf("%s\n", message3);

    			printf("Hit enter to send a message : ");
    			fgets(temp1, 100, stdin);

    			//char message[1000] = "Hi server";
    			status = write(sockfd, message3, strlen(message3));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}
    			printf("Message sent to server\n");

    			char message2[1000];
    			memset(message2, '\0', sizeof(message2));

    			status = read(sockfd, message2, sizeof(message2));
    			if(status == -1){
            			perror("read error\n");
    			}

    			printf("message from server : %s\n", message2);

    			printf("Hit enter to continue in critical section : ");
    			fgets(temp1, 100, stdin);

    			char message4[1000] = "Continue Critical section";
    			status = write(sockfd, message4, strlen(message4));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}

    			memset(message2, '\0', sizeof(message2));

    			status = read(sockfd, message2, sizeof(message2));
    			if(status == -1){
            			perror("read error\n");
    			}

    			printf("message from server : %s\n", message2);

			//printf("Hit enter to continue in critical section : ");
    			fgets(temp1, 100, stdin);

    			char message5[1000] = "UnLock the Lock";
    			status = write(sockfd, message5, strlen(message5));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}
			
		}
		else if(type == 11){
			//11 - Update Price
			char stype[] = "11-";
                        strcat(message3, stype);
                        char productName[100];
                        printf("Enter a Product Name to update it's Price : ");
                        scanf("%[^\n]s", productName);
                        fgets(temp, sizeof(temp), stdin);

                        char quant[10];
                        printf("Enter Updated Price of Product : ");
                        scanf("%[^\n]s", quant);
                        fgets(temp, sizeof(temp), stdin);
                        strcat(message3, productName);
                        strcat(message3, "-");
                        strcat(message3, quant);

			printf("%s\n", message3);

    			printf("Hit enter to send a message : ");
    			fgets(temp1, 100, stdin);

    			//char message[1000] = "Hi server";
    			status = write(sockfd, message3, strlen(message3));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}
    			printf("Message sent to server\n");

    			char message2[1000];
    			memset(message2, '\0', sizeof(message2));

    			status = read(sockfd, message2, sizeof(message2));
    			if(status == -1){
            			perror("read error\n");
    			}

    			printf("message from server : %s\n", message2);

    			printf("Hit enter to continue in critical section : ");
    			fgets(temp1, 100, stdin);

    			char message4[1000] = "Continue Critical section";
    			status = write(sockfd, message4, strlen(message4));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}

    			memset(message2, '\0', sizeof(message2));

    			status = read(sockfd, message2, sizeof(message2));
    			if(status == -1){
            			perror("read error\n");
    			}

    			printf("message from server : %s\n", message2);

			//printf("Hit enter to continue in critical section : ");
    			fgets(temp1, 100, stdin);

    			char message5[1000] = "UnLock the Lock";
    			status = write(sockfd, message5, strlen(message5));
    			if (status == -1)
    			{
        			perror("write error\n");
        			return 1;
    			}
		}
		else if(type == 12){
			//12 - Exit
			//char stype[] = "12";
                        //strcat(message3, stype);
			printf("************************* You are successfully Exited! ***********************\n");
			return 0;
		}
		else{
			printf("Wrong Choice!\n");
		}

    

    		/*
		printf("%s\n", message3);

    		printf("Hit enter to send a message : ");
    		fgets(temp1, 100, stdin);

    		//char message[1000] = "Hi server";
    		status = write(sockfd, message3, strlen(message3));
    		if (status == -1)
    		{
        		perror("write error\n");
        		return 1;
    		}
    		printf("Message sent to server\n");

		*/
    		char message2[1000];
    		memset(message2, '\0', sizeof(message2));
		

    		status = read(sockfd, message2, sizeof(message2));
    		if(status == -1){
	    		perror("read error\n");
    		}

    		printf("message from server : %s\n", message2);

    	}
	
    }

    else{
	    printf("No methods!\n");
    }
    
    /*
    while(1){
    char message[100] = "";
    char temp[100];
    

    printf("%s", message);

    printf("Hit enter to send a message : ");
    fgets(temp, 100, stdin);

    //char message[1000] = "Hi server";
    status = write(sockfd, message, strlen(message));
    if (status == -1)
    {
        perror("write error\n");
        return 1;
    }
    printf("Message sent to server\n");

    char message1[1000];
    status = read(sockfd, message1, sizeof(message1));
    if(status == -1){
	    perror("read error\n");
    }

    printf("message from server : %s\n", message1);

    }*/

    close(sockfd);
    return 0;
}
