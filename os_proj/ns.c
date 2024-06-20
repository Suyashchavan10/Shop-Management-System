#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

struct user{
        int userId;
        char userName[100];
        char password[100];
};

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

int logfd;
int prodfd;
int cartfd;
void addCart(int cartfd, int userId, char* userName){
	int totCarts = 0;
        int start = lseek(cartfd, 0, SEEK_SET);
        read(cartfd, &totCarts, sizeof(int));

	totCarts++;
	start = lseek(cartfd, 0, SEEK_SET);
	write(cartfd, &totCarts, sizeof(int));

	int cur = lseek(cartfd, totCarts*(sizeof(struct cart)), SEEK_CUR);
	struct cart newCart;
	newCart.userId = userId;
	strcpy(newCart.userName, userName);
	int ind = 0;
	for(ind =0; ind<20; ind++){
		newCart.productList[ind].productId = -1;
		char name[100] = "";
		strcpy(newCart.productList[ind].productName, name);
		newCart.productList[ind].quantity = 0;
		newCart.productList[ind].price = 0;
	}
	write(cartfd, &newCart, sizeof(struct cart));
	printf("New Empty Cart added successfully!\n");

	return ;
}

void signIn(int fd, int* totUsers, char name[], char pass[], char result[], int cartfd){
	int id = *(totUsers) + 1;
        int start = lseek(fd, 0, SEEK_SET);
        write(fd, &id, sizeof(int));
        struct user newUser;
        newUser.userId = id;
        strcpy(newUser.userName, name);
        strcpy(newUser.password, pass);
        int end = lseek(fd, (*(totUsers))*(sizeof(struct user)), SEEK_CUR);
        write(fd, &newUser, sizeof(struct user));

        *(totUsers) = *(totUsers) + 1;

	char res[2000] = "0";
	strcpy(result, res);
	addCart(cartfd, id, name);
	return ;
}

void logIn(int fd, char name[], char pass[], char result[]){
	struct user reqUser;
        int start = lseek(fd, sizeof(int), SEEK_SET);
        bool userFound = false;
        while(read(fd, &reqUser, sizeof(struct user))){
                //printf("%d %s %s\n", reqUser.userId, reqUser.userName, reqUser.password);
                if(strcmp(reqUser.userName, name) == 0){
                        userFound = true;
                        break;
                }
        }
	if(userFound){
                if(strcmp(reqUser.password, pass) == 0){
                        char res[2000] = "0";
        		strcpy(result, res);
                        return ;
                }
                else{
                        char res[2000] = "-1";
        		strcpy(result, res);
        		return ;
                }
        }
        else{
                char res[2000] = "-1";
        	strcpy(result, res);
        	return ;
        }

	char res[2000] = "-1";
        strcpy(result, res);
	return ;
}

void displayProducts(int fd, char result[]){
	int totProd = 0;
        int start = lseek(fd, 0, SEEK_SET);
        read(fd, &totProd, sizeof(int));
	
	strcpy(result, "");
        if(totProd > 0){
                struct product prod;
                while(read(fd, &prod, sizeof(struct product))){
                        if(prod.productId != -1){
                                //printf("Product ID : %d, Product Name : %s, Quantity : %d, price : %0.2f\n", prod.productId, prod.productName, prod.quantity, prod.price);
				strcat(result, "Product ID : ");
				char productId[10];
				sprintf(productId, "%d", prod.productId);
				strcat(result, productId);

				strcat(result, ", Product Name : ");
				strcat(result, prod.productName);
				strcat(result, ", Quantity : ");
				char quantity[10];
				sprintf(quantity, "%d", prod.quantity);
				strcat(result, quantity);

				strcat(result, ", Price : ");
				char price[20];
				sprintf(price, "%f", prod.price);
				strcat(result, price);
				strcat(result, "\n");
                        }
                }
     	}

	return;
}

void displayCart(int fd, char userName[], char result[]){
	int totCarts = 0;
        int start = lseek(fd, 0, SEEK_SET);
        read(fd, &totCarts, sizeof(int));

	strcpy(result, "");
        if(totCarts > 0){
                //printf("*********************************** Cart of user : %s *************************************\n", userName);

                struct cart reqCart;
                while(read(fd, &reqCart, sizeof(struct cart))){
                        if(strcmp(reqCart.userName, userName) == 0){
                                int curInd;
                                for(curInd = 0; curInd<20; curInd++){
                                        if(reqCart.productList[curInd].productId != -1){
                                                //printf("Product ID : %d, Product Name : %s, Quantity : %d, price : %0.2f\n", reqCart.productList[curInd].productId, reqCart.productList[curInd].productName, reqCart.productList[curInd].quantity, reqCart.productList[curInd].price);
						strcat(result, "Product ID : ");
                                		char productId[10];
                                		sprintf(productId, "%d", reqCart.productList[curInd].productId);
                                		strcat(result, productId);

                                		strcat(result, ", Product Name : ");
                                		strcat(result, reqCart.productList[curInd].productName);
                                		strcat(result, ", Quantity : ");
                                		char quantity[10];
                                		sprintf(quantity, "%d", reqCart.productList[curInd].quantity);
                                		strcat(result, quantity);

                                		strcat(result, ", Price : ");
                                		char price[20];
                                		sprintf(price, "%f", reqCart.productList[curInd].price);
						strcat(result, price);
                                		strcat(result, "\n");

				       }
                                }
                                break;
                        }
                }

        }

	if(strcmp(result, "") == 0){
		strcpy(result, "Your Cart is Empty!\n");
	}
	return ;
}

void addProductToCart(int fd, int prodfd, char userName[], char productName[], int quant, char result[]){
	int totCarts = 0;
	int start = lseek(fd, 0, SEEK_SET);
	read(fd, &totCarts, sizeof(int));

	strcpy(result, "");
	if(totCarts > 0){
		int totprod = 0;
		int start2 = lseek(prodfd, 0, SEEK_SET);
		read(prodfd, &totprod, sizeof(int));
		
		struct product prod;
		while(read(prodfd, &prod, sizeof(struct product))){
			if(strcmp(prod.productName, productName) == 0){
				if(prod.quantity < quant){
					strcat(result, "Required Quantity is not available!\n");
					//printf("Required Quantity is not available!\n");
					return ;
				}
				break;
			}
		}

		struct cart reqCart;
		int off1 = lseek(fd, sizeof(int), SEEK_SET);
		while(read(fd, &reqCart, sizeof(struct cart))){
			if(strcmp(reqCart.userName, userName) == 0){
				int curInd;
				for(curInd = 0; curInd<20; curInd++){
					if(reqCart.productList[curInd].productId == -1){
						reqCart.productList[curInd].productId = prod.productId;
						strcpy(reqCart.productList[curInd].productName, prod.productName);
						reqCart.productList[curInd].quantity = quant;
						reqCart.productList[curInd].price = prod.price;
						break;
					}
				}
				int off2 = lseek(fd, (-1)*(sizeof(struct cart)), SEEK_CUR);
				write(fd, &reqCart, sizeof(struct cart));

				strcat(result, "New product added to the cart successfully\n");
				//printf("New product added to the cart successfully\n");
				break;
			}
		}
		return;

	}
}

void editCart(int fd, int prodfd, char userName[], char productName[], int quant, char result[]){
	int totCarts = 0;
	int start = lseek(fd, 0, SEEK_SET);
	read(fd, &totCarts, sizeof(int));

	strcpy(result, "");
	if(totCarts > 0){
		int totprod = 0;
		int start2 = lseek(prodfd, 0, SEEK_SET);
		read(prodfd, &totprod, sizeof(int));
		
		struct product prod;
		while(read(prodfd, &prod, sizeof(struct product))){
			if(strcmp(prod.productName, productName) == 0){
				if(prod.quantity < quant){
					strcat(result, "Required Quantity is not available!\n");
					//printf("Required Quantity is not available!\n");
					return ;
				}
				break;
			}
		}

		struct cart reqCart;
		int off1 = lseek(fd, sizeof(int), SEEK_SET);
		while(read(fd, &reqCart, sizeof(struct cart))){
			if(strcmp(reqCart.userName, userName) == 0){
				int curInd;
				for(curInd = 0; curInd<20; curInd++){
					if(strcmp(reqCart.productList[curInd].productName, productName) == 0){
						reqCart.productList[curInd].quantity = quant;
						break;
					}
				}
				int off2 = lseek(fd, (-1)*(sizeof(struct cart)), SEEK_CUR);
				write(fd, &reqCart, sizeof(struct cart));

				strcat(result, "New Quantity of Product is added to the cart successfully\n");
				//printf("New Quantity of Product is added to the cart successfully\n");
				break;
			}
		}
		return ;

	}
	return ;
}

void payment(int cartfd, int prodfd, char userName[], char result[], int csfd){
	int totCarts = 0;
	int status;
        int start = lseek(cartfd, 0, SEEK_SET);
        read(cartfd, &totCarts, sizeof(int));
	strcpy(result, "");
        if(totCarts > 0){
		//printf("****************************** Welcome to PAYMENT page! *******************************\n");
                
		struct cart reqCart;
                int off1 = lseek(cartfd, sizeof(int), SEEK_SET);
                while(read(cartfd, &reqCart, sizeof(struct cart))){
                        if(strcmp(reqCart.userName, userName) == 0){
				//lock the products in the cart
				
				strcat(result, "Inside a critical section of Payment!\n All Products in Your cart are being locked!\n");
				//printf("Inside a critical section of Payment!\n");
				//printf("All Products in Your cart are being locked!\n");
				struct flock locks[20];
				int totLocks = 0;
				int ind;

				for(ind = 0; ind<20; ind++){
					if(reqCart.productList[ind].productId != -1){
                                               	int off3 = lseek(prodfd, sizeof(int), SEEK_SET);
                                               	struct product prod1;
                                               	while(read(prodfd, &prod1, sizeof(struct product))){
                                                       	if(prod1.productId != -1 && (strcmp(prod1.productName, reqCart.productList[ind].productName) == 0) && reqCart.productList[ind].quantity <= prod1.quantity){
								//acquire a lock first on product then reduce quantity
                                                		locks[totLocks].l_type = F_WRLCK;
                                                		locks[totLocks].l_whence = SEEK_CUR;
                                                		locks[totLocks].l_start = (-1)*(sizeof(struct product));
                                                		locks[totLocks].l_len = sizeof(struct product);
                                                		locks[totLocks].l_pid = getpid();
                                                		fcntl(prodfd, F_SETLKW, &locks[totLocks]);
                                                		totLocks++;
                                                               	break;
                                                       	}	
                                             	}

					}
				}
				

				strcat(result, "$-$-$-$-$ Your Receipt : $-$-$-$-$\n");
				//printf("$-$-$-$-$ Your Receipt : $-$-$-$-$\n");

                                int curInd;
				float totamt = 0;
                                for(curInd = 0; curInd<20; curInd++){
                                        if(reqCart.productList[curInd].productId != -1){
                                        	int off2 = lseek(prodfd, sizeof(int), SEEK_SET);
						struct product prod;
						while(read(prodfd, &prod, sizeof(struct product))){
							if(prod.productId != -1 && (strcmp(prod.productName, reqCart.productList[curInd].productName) == 0) && reqCart.productList[curInd].quantity <= prod.quantity){
								//printf("Product Id : %d, Product Name : %s, Quantity : %d, Price : %0.2f\n", prod.productId, prod.productName, reqCart.productList[curInd].quantity, prod.price);	
								strcat(result, "Product ID : ");
                                				char productId[10];
                                				sprintf(productId, "%d", prod.productId);
                                				strcat(result, productId);

                                				strcat(result, ", Product Name : ");
                                				strcat(result, prod.productName);
                                				strcat(result, ", Quantity : ");
                                				char quantity[10];
                                				sprintf(quantity, "%d", reqCart.productList[curInd].quantity);
                                				strcat(result, quantity);

                                				strcat(result, ", Price : ");
                                				char price[20];
                                				sprintf(price, "%f", prod.price);
								strcat(result, price);
                                				strcat(result, "\n");

								totamt += prod.price * reqCart.productList[curInd].quantity;
								break;
							}
						}
                                        }
                                }

				char total[20];
				sprintf(total, "%f", totamt);
				strcat(result, "$-$-$-$-$ Total Amount : ");
				strcat(result, total);
				strcat(result, " $-$-$-$-$\n");
				//printf("$-$-$-$-$ Total Amount : %0.2f $-$-$-$-$\n", totamt);
				float amtPaid = 0;
				strcat(result, "Pay amount : ");
				//printf("Pay amount : ");
				status = write(csfd, result, strlen(result));

        			char req[100];
        			memset(req, '\0', sizeof(req));
        			status = read(csfd, req, sizeof(req));
				sscanf(req, "%f", &amtPaid);

				//scanf("%f", &amtPaid);
				//fgets(temp, sizeof(temp), stdin);

				strcpy(result, "");
				bool paid = false;
				if(amtPaid >= totamt){
					//Now do empty the cart of current user after successful payment
					paid = true;
					for(ind = 0; ind<20; ind++){
						if(reqCart.productList[ind].productId != -1){
                                                	int off3 = lseek(prodfd, sizeof(int), SEEK_SET);
                                                	struct product prod2;
                                                	while(read(prodfd, &prod2, sizeof(struct product))){
                                                        	if(prod2.productId != -1 && (strcmp(prod2.productName, reqCart.productList[ind].productName) == 0) && reqCart.productList[ind].quantity <= prod2.quantity){
									prod2.quantity -= reqCart.productList[ind].quantity;

                                                        		int off4 = lseek(prodfd, (-1)*(sizeof(struct product)), SEEK_CUR);
                                                        		write(prodfd, &prod2, sizeof(struct product));
	
                                                        		reqCart.productList[ind].productId = -1;
                                                        		char tName[100] = "";
                                                        		strcpy(reqCart.productList[ind].productName, tName);
                                                        		reqCart.productList[ind].quantity = 0;
                                                        		reqCart.productList[ind].price = 0;
                                                                	break;
                                                        	}	
                                                	}
						}
					}
					int off5 = lseek(cartfd, (-1)*(sizeof(struct cart)), SEEK_CUR);
                                	write(cartfd, &reqCart, sizeof(struct cart));

					strcat(result, "Payment Successful!\n Your Cart is now empty!\n");
					//printf("Payment Successful!\n");
					//printf("Your Cart is now empty!\n");
				}
				else{
					strcat(result, "Payable Amount is greater!\n");
					//printf("Payable Amount is greater!\n");
				}

				strcat(result, "End of critical section of Payment!\n");
				//printf("End of critical section of Payment!\n");

				strcat(result, "Press Enter to unlock All Locks : ");
                                //printf("Press Enter to unlock All Locks : ");
                                //fgets(temp, sizeof(temp), stdin);

				status = write(csfd, result, strlen(result));

                                char req2[100];
                                memset(req2, '\0', sizeof(req2));
                                status = read(csfd, req2, sizeof(req2));

                                int lind;
                                for(lind=0; lind<totLocks; lind++){
                                        locks[lind].l_type = F_UNLCK;
                                        fcntl(prodfd, F_SETLK, &locks[lind]);
                                }

				strcpy(result, "");
				strcat(result, "All Locks are unlocked!\n");
                                //printf("All Locks are unlocked!\n");

				if(paid){
					return ;
				}
				else{
					return ;
				}

                        }
                }
                return ;
	}
	return ;
}

void addProduct(int fd, int* totprod, char prodName[], int quant, float price, char result[], int csfd){
	int prodId = 0;
	int status;
	prodId = (*totprod) + 1;
	int start = lseek(fd, 0, SEEK_SET);
	write(fd, &prodId, sizeof(int));
	//int end = lseek(fd, (*totprod)*(sizeof(struct product)), SEEK_CUR);

	struct product prod;
	prod.productId = prodId;
	strcpy(prod.productName, prodName);
	prod.quantity = quant;
	prod.price = price;

	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = (*totprod)*(sizeof(struct product)) + sizeof(int);
	lock.l_len = sizeof(struct product);
	lock.l_pid = getpid();

	strcpy(result, "");
        strcat(result, "Inside a critical section of product : ");
        strcat(result, prodName);
        strcat(result, "\n");

        status = write(csfd, result, strlen(result));

        char req[100];
	memset(req, '\0', sizeof(req));
        status = read(csfd, req, sizeof(req));                          
	printf("%s\n", req);

	//printf("Inside a critical section of product : %s\n", prodName);
	fcntl(fd, F_SETLKW, &lock);
	int end = lseek(fd, (*totprod)*(sizeof(struct product)) + sizeof(int), SEEK_SET);
	write(fd, &prod, sizeof(struct product));

	strcpy(result, "");
        strcat(result, "End of critical section of product : ");
        strcat(result, prodName);
	//printf("End of critical section of product : %s\n", prodName);
	
	strcat(result, "\n Press Enter to unlock : ");

        status = write(csfd, result, strlen(result));

        char req2[100];
	memset(req2, '\0', sizeof(req2));
        status = read(csfd, req2, sizeof(req2));                        
	printf("%s\n", req2);
	//printf("Press Enter to unlock : ");
	//fgets(temp, sizeof(temp), stdin);
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);

	strcpy(result, "");
        strcat(result, "Lock is unlocked!\n Product is Added successfully!\n");
	//printf("Lock is unlocked!\n");

	*totprod = (*totprod) + 1;
	//printf("Product added successfully!\n");

        return ;
}

void deleteProduct(int fd, char prodName[], char result[], int csfd){
	struct product prod;
	int status;
	int start = lseek(fd, sizeof(int), SEEK_SET);
	while(read(fd, &prod, sizeof(struct product))){
		if(strcmp(prod.productName, prodName) == 0){
			struct flock lock;
        		lock.l_type = F_WRLCK;
        		lock.l_whence = SEEK_CUR;
        		lock.l_start = (-1)*(sizeof(struct product));
        		lock.l_len = sizeof(struct product);
        		lock.l_pid = getpid();

			strcpy(result, "");
			strcat(result, "Inside a critical section of product : ");
			strcat(result, prodName);
			strcat(result, "\n");

			status = write(csfd, result, strlen(result));
			 
			char req[100];
			memset(req, '\0', sizeof(req));
			status = read(csfd, req, sizeof(req));				
			printf("%s\n", req);
    			
			//printf("Inside a critical section of product : %s\n", prodName);
        		fcntl(fd, F_SETLKW, &lock);
			prod.productId = -1;
			char temp1[100] = "";
			strcpy(prod.productName, temp1);
		       	prod.quantity = 0;
			prod.price = 0;
			int off1 = lseek(fd, (-1)*(sizeof(struct product)), SEEK_CUR);
			write(fd, &prod, sizeof(struct product));	

			strcpy(result, "");
			strcat(result, "End of critical section of product : ");
			strcat(result, prodName);
			//printf("End of critical section of product : %s\n", prodName);
			strcat(result, "\n Press Enter to unlock : ");

			status = write(csfd, result, strlen(result));

			char req2[100];
			memset(req2, '\0', sizeof(req2));
			status = read(csfd, req2, sizeof(req2));			
			printf("%s\n", req2);

        		//printf("Press Enter to unlock : ");
        		//fgets(temp, sizeof(temp), stdin);
        		lock.l_type = F_UNLCK;
        		fcntl(fd, F_SETLK, &lock);

			strcpy(result, "");
			strcat(result, "Lock is unlocked!\n Product is deleted successfully!\n");
        		//printf("Lock is unlocked!\n");
			//printf("Product is deleted successfully!\n");
			return ;
		}
	}

	strcpy(result, "");
	strcat(result, "Product is not found!\n");
	//printf("Product is not found!\n");
	return ;
}

void updateQuantity(int fd, char prodName[], int quant, char result[], int csfd){
	int status;
	int start = lseek(fd, sizeof(int), SEEK_SET);

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

		strcpy(result, "");
                strcat(result, "Inside a critical section of product : ");
                strcat(result, prodName);
                strcat(result, "\n");

                status = write(csfd, result, strlen(result));

                char req[100];
		memset(req, '\0', sizeof(req));
                status = read(csfd, req, sizeof(req));                          
		printf("%s\n", req);
                //printf("Inside a critical section of product : %s\n", prodName);
                fcntl(fd, F_SETLKW, &lock);

		int cur = lseek(fd, (-1)*sizeof(struct product), SEEK_CUR);

		prod.quantity = quant;
		write(fd, &prod, sizeof(struct product));

		strcpy(result, "");
                strcat(result, "End of critical section of product : ");
                strcat(result, prodName);

		//printf("End of critical section of product : %s\n", prodName);

		strcat(result, "\n Press Enter to unlock : ");
		status = write(csfd, result, strlen(result));

                char req2[100];
		memset(req2, '\0', sizeof(req2));
                status = read(csfd, req2, sizeof(req2));                        
		printf("%s\n", req2);
                //printf("Press Enter to unlock : ");
		//fgets(temp, sizeof(temp), stdin);
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);

		strcpy(result, "");
                strcat(result, "Lock is unlocked!\n Product Quantity updated successfully!\n");
                //printf("Lock is unlocked!\n");

		//printf("Product Quantity updated successfully!\n");
		return ;
	}
	else{
		strcpy(result, "");
        	strcat(result, "Product is not found!\n");
		//printf("Product Name not found\n");
		return ;
	}

        return ;
}

void updatePrice(int fd, char prodName[], float price, char result[], int csfd){
	int status;
        int start = lseek(fd, sizeof(int), SEEK_SET);

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

		strcpy(result, "");
                strcat(result, "Inside a critical section of product : ");
                strcat(result, prodName);
                strcat(result, "\n");

                status = write(csfd, result, strlen(result));

                char req[100];
		memset(req, '\0', sizeof(req));
                status = read(csfd, req, sizeof(req));                          
		printf("%s\n", req);

                //printf("Inside a critical section of product : %s\n", prodName);
                fcntl(fd, F_SETLKW, &lock);

		int cur = lseek(fd, (-1)*sizeof(struct product), SEEK_CUR);
	
		prod.price = price;
		write(fd, &prod, sizeof(struct product));

		strcpy(result, "");
                strcat(result, "End of critical section of product : ");
                strcat(result, prodName);

		//printf("End of critical section of product : %s\n", prodName);

		strcat(result, "\n Press Enter to unlock : ");
                status = write(csfd, result, strlen(result));

                char req2[100];
		memset(req2, '\0', sizeof(req2));
                status = read(csfd, req2, sizeof(req2));                        
		printf("%s\n", req2);
                //printf("Press Enter to unlock : ");
                //fgets(temp, sizeof(temp), stdin);
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);

		strcpy(result, "");
                strcat(result, "Lock is unlocked!\n Product Price updated successfully!\n");
                //printf("Lock is unlocked!\n");
		//printf("Product Price updated successfully!\n");
		return ;
	}
	else{
		strcpy(result, "");
                strcat(result, "Product is not found!\n");
		//printf("Product Name not found\n");
		return ;
	}
        return ;
}

void RequestHandler(char req[], char result[], int* totUsers, int* totprod, int csfd){
	char* reqType = strtok(req, "-");
	//LOGIN AND SIGNIN
	if(strcmp(reqType,"1") == 0){
		//LogIn
		char* clientType = strtok(NULL, "-");
		char* userName = strtok(NULL, "-");
		char* password = strtok(NULL, "-");
		if(strcmp(clientType, "1") == 0){
			//User
		}
		else if(strcmp(clientType, "2") == 0){
			//Admin
		}

		logIn(logfd, userName, password, result);
	}
	else if(strcmp(reqType,"2") == 0){
		//SignIn
		char* clientType = strtok(NULL, "-");
                char* userName = strtok(NULL, "-");
                char* password = strtok(NULL, "-");
                if(strcmp(clientType, "1") == 0){
                        //User
                }
                else if(strcmp(clientType, "2") == 0){
                        //Admin
                }

		signIn(logfd, totUsers, userName, password, result, cartfd);
	}

	//USER METHODS
	else if(strcmp(reqType,"3") == 0){
		//Display Product
		displayProducts(prodfd, result);
	}
	else if(strcmp(reqType,"4") == 0){
		//Display Cart
		char* userName = strtok(NULL, "-");

		displayCart(cartfd, userName, result);
	}
	else if(strcmp(reqType,"5") == 0){
		//Add Product to Cart
		char* userName = strtok(NULL, "-");
		char* productName = strtok(NULL, "-");
		char* quantity = strtok(NULL, "-");
		
	int quant;
		sscanf(quantity, "%d", &quant);
		addProductToCart(cartfd, prodfd, userName, productName, quant, result);
	}
	else if(strcmp(reqType,"6") == 0){
		//Edit Cart
		char* userName = strtok(NULL, "-");
                char* productName = strtok(NULL, "-");
                char* quantity = strtok(NULL, "-");

		int quant;
                sscanf(quantity, "%d", &quant);
		editCart(cartfd, prodfd, userName, productName, quant, result);
	}
	else if(strcmp(reqType,"7") == 0){
		//Payment
		char* userName = strtok(NULL, "-");

		payment(cartfd, prodfd, userName, result, csfd);
	}

	//ADMIN METHODS
	else if(strcmp(reqType,"8") == 0){
		//Add Product
		char* productName = strtok(NULL, "-");
                char* quantity = strtok(NULL, "-");
		char* price = strtok(NULL, "-");
		
		int quant;
                sscanf(quantity, "%d", &quant);
		float priceInt;
		sscanf(price, "%f", &priceInt);
		addProduct(prodfd, totprod, productName, quant, priceInt, result, csfd);
	}
	else if(strcmp(reqType,"9") == 0){
		//Delete Product
		char* productName = strtok(NULL, "-");

		deleteProduct(prodfd, productName, result, csfd);
	}
	else if(strcmp(reqType,"10") == 0){
		//Update Quantity
		char* productName = strtok(NULL, "-");
                char* quantity = strtok(NULL, "-");

		int quant;
                sscanf(quantity, "%d", &quant);
		updateQuantity(prodfd, productName, quant, result, csfd);
	}
	else if(strcmp(reqType,"11") == 0){
		//Update Price
		char* productName = strtok(NULL, "-");
		char* price = strtok(NULL, "-");

		float priceInt;
                sscanf(price, "%f", &priceInt);
		updatePrice(prodfd, productName, priceInt, result, csfd);
	}
	else{
		//Wrong Type
	}

	//return result;
}

int totUsers = 0;
int totprod = 0;
int totcart = 0;

void *handleClient(void *arg)
{

    int clientSockfd = *(int *)arg;
    int cur = totUsers;
    char buffer[1000];
    char request[1000];
    char response[2000];
    while(1){
    memset(buffer, '\0', sizeof(buffer));
    memset(request, '\0', sizeof(request));

    int status = read(clientSockfd, buffer, sizeof(buffer));
    if (status == -1)
    {
        perror("read error\n");
        return NULL;
    }
    strcpy(request, buffer);
    printf("Message from client %d: ", cur);
    char * token = strtok(buffer, "-");
    // loop through the string to extract all other tokens
    while( token != NULL ) {
        printf( "%s ", token ); //printing each token
        token = strtok(NULL, "-");
    }
    printf("\n");

    // Get appropriate Response to Request from Client by calling corresponding methods and then write this Response to the socket.

    memset(response, '\0', sizeof(response));
    RequestHandler(request, response, &totUsers, &totprod, clientSockfd);

    printf("%s\n", response);
    status = write(clientSockfd, response, strlen(response));
    if(status == -1){
	    perror("Write error\n");
    }

    }

    close(clientSockfd);
    return NULL;
}

int main()
{

	prodfd = open("products", O_EXCL | O_RDWR, 0744);
        if(prodfd == -1){
                perror("Error in products file opening\n");
                return 0;
        }

	logfd = open("login", O_EXCL | O_RDWR, 0744);
        if(logfd == -1){
                perror("Error in login file opening\n");
                return 0;
        }

	cartfd = open("carts", O_EXCL | O_RDWR, 0744);
        if(cartfd == -1){
                perror("Error in carts file opening\n");
                return 0;
        }
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

    int off1;
    totprod = 0;
    off1 = lseek(prodfd, 0, SEEK_SET);
    read(prodfd, &totprod, sizeof(int));
    printf("total Products : %d\n", totprod);

    totUsers = 0;
    off1 = lseek(logfd, 0, SEEK_SET);
    read(logfd, &totUsers, sizeof(int));
    printf("total Users : %d\n", totUsers);

    totcart = 0;
    off1 = lseek(cartfd, 0, SEEK_SET);
    read(cartfd, &totcart, sizeof(int));
    printf("total Carts : %d\n", totcart);

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
	totUsers++;
        status = pthread_create(&thread, NULL, handleClient, &clientSockfd);
        if (status != 0)
        {
            perror("pthread_create");
            return 1;
        }
        pthread_detach(thread);
       	// ! detach the thread so that it can be cleaned up automatically
    }

    close(prodfd);
    close(logfd);
    close(cartfd);
    return 0;
}
