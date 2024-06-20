#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

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

void displayProducts(int fd){
	int totProd = 0;
	int start = lseek(fd, 0, SEEK_SET);
	read(fd, &totProd, sizeof(int));
	if(totProd > 0){
		printf("******************************* Products list *********************************\n");
		struct product prod;
		while(read(fd, &prod, sizeof(struct product))){
			if(prod.productId != -1){
				printf("Product ID : %d, Product Name : %s, Quantity : %d, price : %0.2f\n", prod.productId, prod.productName, prod.quantity, prod.price);
			}
		}
	}		

	return;
}

int addProduct(int fd, int *totprod){
	int prodId = 0;
	prodId = (*totprod) + 1;
	int start = lseek(fd, 0, SEEK_SET);
	write(fd, &prodId, sizeof(int));
	//int end = lseek(fd, (*totprod)*(sizeof(struct product)), SEEK_CUR);

	struct product prod;
	prod.productId = prodId;
	printf("Enter Product name to add: ");
	char prodName[100];
	scanf("%[^\n]s", prodName);
	char temp[100];
	fgets(temp, sizeof(temp), stdin);
	strcpy(prod.productName, prodName);

	int quant = 0;
	printf("Enter Quantity of product : ");
	scanf("%d", &quant);
	fgets(temp, sizeof(temp), stdin);
	prod.quantity = quant;

	float price = 0;
	printf("Enter Price of product : ");
	scanf("%f", &price);
	fgets(temp, sizeof(temp), stdin);
	prod.price = price;

	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = (*totprod)*(sizeof(struct product)) + sizeof(int);
	lock.l_len = sizeof(struct product);
	lock.l_pid = getpid();

	printf("Inside a critical section of product : %s\n", prodName);
	fcntl(fd, F_SETLKW, &lock);
	int end = lseek(fd, (*totprod)*(sizeof(struct product)) + sizeof(int), SEEK_SET);
	write(fd, &prod, sizeof(struct product));
	printf("End of critical section of product : %s\n", prodName);
	
	printf("Press Enter to unlock : ");
	fgets(temp, sizeof(temp), stdin);
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	printf("Lock is unlocked!\n");

	*totprod = (*totprod) + 1;
	printf("Product added successfully!\n");
	return 0;
}

int deleteProduct(int fd){
	int prodId = 0;
        int start = lseek(fd, 0, SEEK_SET);
        read(fd, &prodId, sizeof(int));

	printf("Enter Product name to delete: ");
        char prodName[100];
        scanf("%[^\n]s", prodName);
        char temp[100];
        fgets(temp, sizeof(temp), stdin);

	struct product prod;
	start = lseek(fd, sizeof(int), SEEK_SET);
	while(read(fd, &prod, sizeof(struct product))){
		if(strcmp(prod.productName, prodName) == 0){
			struct flock lock;
        		lock.l_type = F_WRLCK;
        		lock.l_whence = SEEK_CUR;
        		lock.l_start = (-1)*(sizeof(struct product));
        		lock.l_len = sizeof(struct product);
        		lock.l_pid = getpid();

			printf("Inside a critical section of product : %s\n", prodName);
        		fcntl(fd, F_SETLKW, &lock);
			prod.productId = -1;
			char temp1[100] = "";
			strcpy(prod.productName, temp1);
		       	prod.quantity = 0;
			prod.price = 0;
			int off1 = lseek(fd, (-1)*(sizeof(struct product)), SEEK_CUR);
			write(fd, &prod, sizeof(struct product));	
			printf("End of critical section of product : %s\n", prodName);

        		printf("Press Enter to unlock : ");
        		fgets(temp, sizeof(temp), stdin);
        		lock.l_type = F_UNLCK;
        		fcntl(fd, F_SETLK, &lock);
        		printf("Lock is unlocked!\n");

			printf("Product is deleted successfully!\n");
			return 0;
		}
	}

	printf("Product is not found!\n");
	return -1;
}

int updateQuantity(int fd){
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

int updatePrice(int fd){
	int start = lseek(fd, sizeof(int), SEEK_SET);
	char prodName[100];
	printf("Enter a Product Name to update it's Price : ");
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
		float price = 0;
		printf("Enter Updated Price of Product : ");
		scanf("%f", &price);
		fgets(temp, sizeof(temp), stdin);
		prod.price = price;
		write(fd, &prod, sizeof(struct product));

		printf("End of critical section of product : %s\n", prodName);

                printf("Press Enter to unlock : ");
                fgets(temp, sizeof(temp), stdin);
                lock.l_type = F_UNLCK;
                fcntl(fd, F_SETLK, &lock);
                printf("Lock is unlocked!\n");

		printf("Product Price updated successfully!\n");
		return 0;
	}
	else{
		printf("Product Name not found\n");
		return -1;
	}
	return 0;
}

int paymentl(int cartfd, int prodfd){
	int totCarts = 0;
        int start = lseek(cartfd, 0, SEEK_SET);
        read(cartfd, &totCarts, sizeof(int));
        if(totCarts > 0){
		printf("****************************** Welcome to PAYMENT page! *******************************\n");
                char userName[100];
                printf("Enter a user name to proceed for Payment : ");
                scanf("%[^\n]s", userName);
                char temp[100];
                fgets(temp, sizeof(temp), stdin);


		struct cart reqCart;
                int off1 = lseek(cartfd, sizeof(int), SEEK_SET);
                while(read(cartfd, &reqCart, sizeof(struct cart))){
                        if(strcmp(reqCart.userName, userName) == 0){
				printf("$-$-$-$-$ Your Receipt : $-$-$-$-$\n");

                                int curInd;
				float totamt = 0;
                                for(curInd = 0; curInd<20; curInd++){
                                        if(reqCart.productList[curInd].productId != -1){
                                        	int off2 = lseek(prodfd, sizeof(int), SEEK_SET);
						struct product prod;
						while(read(prodfd, &prod, sizeof(struct product))){
							if(prod.productId != -1 && (strcmp(prod.productName, reqCart.productList[curInd].productName) == 0) && reqCart.productList[curInd].quantity <= prod.quantity){
								printf("Product Id : %d, Product Name : %s, Quantity : %d, Price : %0.2f\n", prod.productId, prod.productName, reqCart.productList[curInd].quantity, prod.price);	
								totamt += prod.price;
								break;
							}
						}
                                        }
                                }
				printf("$-$-$-$-$ Total Amount : %0.2f $-$-$-$-$\n", totamt);
				float amtPaid = 0;
				printf("Pay amount : ");
				scanf("%f", &amtPaid);
				fgets(temp, sizeof(temp), stdin);

				if(amtPaid >= totamt){
					int ind;
					//Now do empty the cart of current user after successful payment
					//also lock the products in the cart

					printf("Inside a critical section of Payment!\n");
					printf("All Products in Your cart are being locked!\n");
					struct flock locks[20];
					int totLocks = 0;

					for(ind = 0; ind<20; ind++){
						if(reqCart.productList[ind].productId != -1){
                                                	int off3 = lseek(prodfd, sizeof(int), SEEK_SET);
                                                	struct product prod1;
                                                	while(read(prodfd, &prod1, sizeof(struct product))){
                                                        	if(prod1.productId != -1 && (strcmp(prod1.productName, reqCart.productList[ind].productName) == 0) && reqCart.productList[ind].quantity <= prod1.quantity){
                                                                	break;
                                                        	}	
                                                	}

							//acquire a lock first on product then reduce quantity
                                                        locks[totLocks].l_type = F_WRLCK;
                                                        locks[totLocks].l_whence = SEEK_CUR;
                                                        locks[totLocks].l_start = (-1)*(sizeof(struct product));
                                                        locks[totLocks].l_len = sizeof(struct product);
                                                        locks[totLocks].l_pid = getpid();
                                                        fcntl(prodfd, F_SETLKW, &locks[totLocks]);
							totLocks++;

                                                        prod1.quantity -= reqCart.productList[ind].quantity;

							int off4 = lseek(prodfd, (-1)*(sizeof(struct product)), SEEK_CUR);
                                        		write(prodfd, &prod1, sizeof(struct product));

							reqCart.productList[ind].productId = -1;
							char tName[100] = "";
							strcpy(reqCart.productList[ind].productName, tName);
							reqCart.productList[ind].quantity = 0;
							reqCart.productList[ind].price = 0;
						}
					}
					int off5 = lseek(cartfd, (-1)*(sizeof(struct cart)), SEEK_CUR);
                                	write(cartfd, &reqCart, sizeof(struct cart));

					printf("End of critical section of Payment!\n");

                			printf("Press Enter to unlock All Locks : ");
                			fgets(temp, sizeof(temp), stdin);
                			
					int lind;
					for(lind=0; lind<totLocks; lind++){
						locks[lind].l_type = F_UNLCK;
                                        	fcntl(prodfd, F_SETLK, &locks[lind]);
					}
                			printf("All Locks are unlocked!\n");

					printf("Payment Successful!\n");
					printf("Your Cart is now empty!\n");

					return 0;
				}
				else{
					printf("Payable Amount is greater!\n");
					return -1;
				}

                        }
                }
                return -1;
	}
	return -1;
}


int main(){
	//printf("I am in pt file\n");
	
	int prodfd;
	prodfd = open("prod", O_EXCL | O_RDWR, 0744);
	if(prodfd == -1){
		perror("Error in login file opening\n");
		return 0;
	}
	//printf("logfd : %d\n", logfd);}
	
	int totProds = 0;
	//write system call is used at starting to initialise totProds to 0.
	//int off2 = lseek(prodfd, 0, SEEK_SET);
	//write(prodfd, &totProds, sizeof(int));

	
	int off1 = lseek(prodfd, 0, SEEK_SET);
	read(prodfd, &totProds, sizeof(int));
	printf("total Products : %d\n", totProds);

	//displayProducts(prodfd);
	//int stat1 = addProduct(prodfd, &totProds);
	//displayProducts(prodfd);
	//int stat2 = updateQuantity(prodfd);
	//int stat3 = updatePrice(prodfd);
	//
	//displayProducts(prodfd);
	//int stat8 = addProduct(prodfd, &totProds);
	//int stat9 = addProduct(prodfd, &totProds);
	
	displayProducts(prodfd);
	//int stat7 = deleteProduct(prodfd);
	dint stat9 = updateQuantity(prodfd);
	displayProducts(prodfd);
	

	close(prodfd);
	return 0;
}
