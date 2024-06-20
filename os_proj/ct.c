#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

void displayProducts(int fd){
	int totProd = 0;
	int start = lseek(fd, 0, SEEK_SET);
	read(fd, &totProd, sizeof(int));
	if(totProd > 0){
		printf("******************************* Products list *********************************\n");
		struct product prod;
		while(read(fd, &prod, sizeof(struct product))){
			printf("Product ID : %d, Product Name : %s, Quantity : %d, price : %0.2f\n", prod.productId, prod.productName, prod.quantity, prod.price);
		}
	}		

	return;
}

int addProduct(int fd, int *totprod){
	int prodId = 0;
	prodId = (*totprod) + 1;
	int start = lseek(fd, 0, SEEK_SET);
	write(fd, &prodId, sizeof(int));
	int end = lseek(fd, (*totprod)*(sizeof(struct product)), SEEK_CUR);

	struct product prod;
	prod.productId = prodId;
	printf("Enter Product name : ");
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

	write(fd, &prod, sizeof(struct product));

	*totprod = (*totprod) + 1;
	printf("Product added successfully!\n");
	return 0;
}

int deleteProduct(int fd){
	return 0;
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
		int cur = lseek(fd, (-1)*sizeof(struct product), SEEK_CUR);
		int quant = 0;
		printf("Enter Updated Quantity of Product : ");
		scanf("%d", &quant);
		fgets(temp, sizeof(temp), stdin);
		prod.quantity = quant;
		write(fd, &prod, sizeof(struct product));
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
		int cur = lseek(fd, (-1)*sizeof(struct product), SEEK_CUR);
		float price = 0;
		printf("Enter Updated Price of Product : ");
		scanf("%f", &price);
		fgets(temp, sizeof(temp), stdin);
		prod.price = price;
		write(fd, &prod, sizeof(struct product));
		printf("Product Price updated successfully!\n");
		return 0;
	}
	else{
		printf("Product Name not found\n");
		return -1;
	}
	return 0;
}


int main(){
	//printf("I am in pt file\n");
	
	int prodfd;
	prodfd = open("products", O_EXCL | O_RDWR, 0744);
	if(prodfd == -1){
		perror("Error in login file opening\n");
		return 0;
	}
	//printf("logfd : %d\n", logfd);
	
	int totProds = 0;
	//write system call is used at starting to initialise totProds to 0.
	//int off2 = lseek(prodfd, 0, SEEK_SET);
	//write(prodfd, &totProds, sizeof(int));

	
	int off1 = lseek(prodfd, 0, SEEK_SET);
	read(prodfd, &totProds, sizeof(int));
	printf("total Products : %d\n", totProds);

	displayProducts(prodfd);
	int stat1 = addProduct(prodfd, &totProds);
	displayProducts(prodfd);
	int stat2 = updateQuantity(prodfd);
	int stat3 = updatePrice(prodfd);
	displayProducts(prodfd);

	close(prodfd);
	return 0;
}
