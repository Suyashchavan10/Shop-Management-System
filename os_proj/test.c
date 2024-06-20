#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(){
	char buffer[100] = "1-2-Suyash chavan-12 34";
	char* token = strtok(buffer, "-");
    // loop through the string to extract all other tokens
    /*while( token != NULL ) {
        printf( "%s ", token ); //printing each token
        token = strtok(NULL, "-");
    }*/
	printf("%s %s\n", token, buffer);
	char* t1 = strtok(NULL, "-");
	printf("%s %s\n", t1, buffer);
	char* t2 = strtok(NULL, "-");
        printf("%s %s\n", t2, buffer);
	char* t3 = strtok(NULL, "-");
        printf("%s %s\n", t3, buffer);

	return 0;
}	
