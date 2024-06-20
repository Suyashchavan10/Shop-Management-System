#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>


struct user{
	int userId;
	char userName[100];
	char password[100];
};

int signIn(int fd, int* totUsers){
	printf("Welcome to SIGNIN page\n");
	printf("Enter user name : ");
	char name[100];
	char temp[100];
	//fgets(name, sizeof(name), stdin);
	//fgets(temp, sizeof(temp), stdin);
	
	scanf("%[^\n]s",name);
	//printf("%s\n", name);
	fgets(temp, sizeof(temp), stdin);
	
	char pass[100];
	printf("Set password to your account : ");
	scanf("%[^\n]s", pass);
	fgets(temp, sizeof(temp), stdin);

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
	printf("SIGNIN successful!\n");

	return 0;
}

int logIn(int fd){
	printf("Welcome to LOGIN page\n");
	printf("Enter user name : ");
	char name[100];
	char temp[100];
	
	scanf("%[^\n]s",name);
	//printf("%s\n", name);
	fgets(temp, sizeof(temp), stdin);

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
		char pass[100];
		printf("Enter password to your account : ");
		scanf("%[^\n]s", pass);
		fgets(temp, sizeof(temp), stdin);
		if(strcmp(reqUser.password, pass) == 0){
			printf("LOGIN successful!\n");
			return 0;
		}
		else{
			printf("Wrong password!\n");
			return -1;
		}
	}
	else{
		printf("User Name not found, Please SIGNIN first!\n");
		return -1;
	}

	return 0;
}

int main(){
	//printf("I am in lt file\n");
	
	int logfd;
	logfd = open("login", O_EXCL | O_RDWR, 0744);
	if(logfd == -1){
		perror("Error in login file opening\n");
		return 0;
	}
	//printf("logfd : %d\n", logfd);
	
	int totUsers = 0;
	//write system call is used at starting to initialise totUsers to 0.
	//int off2 = lseek(logfd, 0, SEEK_SET);
	//write(logfd, &totUsers, sizeof(int));

	
	int off1 = lseek(logfd, 0, SEEK_SET);
	read(logfd, &totUsers, sizeof(int));
	printf("total users : %d\n", totUsers);

	//signIn(logfd, &totUsers);
	//signIn(logfd, &totUsers);

	//to print last signed in user info
	/*
	int off = lseek(logfd, (-1)*sizeof(struct user), SEEK_END);
	struct user last;
	read(logfd, &last, sizeof(struct user));
	printf("%d %s %s \n", last.userId, last.userName, last.password);
	*/


	int logStatus = logIn(logfd);
	printf("LOGIN status : %d\n", logStatus);

	close(logfd);
	return 0;
}
