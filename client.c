#include "mysocket.h"
#include <pthread.h>
#include "RSALibrary.h"

struct Arguments {
    int connfd;
    int my_e;
    int my_c;
    int my_d;
    int you_e;
    int you_c;
};


void sendKeys( struct Arguments *args){
    send(args->connfd, &args->my_e, sizeof(int), 0);
    send(args->connfd, &args->my_c, sizeof(int), 0);
    printf("My public key: %d %d\n", args->my_e, args->my_c);
    printf("My private key: %d, %d\n", args->my_d, args->my_c);
}


void receiveKeys(struct Arguments *args) {
    read(args->connfd, &args->you_e, sizeof(int));
    read(args->connfd, &args->you_c, sizeof(int));
    printf("Friends' public key : %d %d\n", args->you_e, args->you_c);
}

void *readData(void * argument) {
    struct Arguments *args = (struct Arguments *)argument;
    
    char buffer[256];
    while(1) {
        read(args->connfd, buffer, sizeof(buffer));
	puts(buffer);
        if (strcmp("quit", buffer) == 0) {
	   write(args->connfd, buffer, strlen(buffer));
           return;
        }
        bzero(buffer, 256);
    }

}

void *writeData(void* argument) {
    
    struct Arguments *args = (struct Arguments*)argument;
    
    char buffer[256];
    int encBuffer[256];
    int i = 0;
    int j = 0; 
    int bufferSize = 0;
    while(1) {
        i =  0; 
        while(1) {
            buffer[i] = getchar();
            if(buffer[i] == '\n') {
                encBuffer[i] = '\0';
                buffer[i] = '\0';  
                encBuffer[i] = encrypt(buffer[i], args->you_e, args->you_c); 
                bufferSize = i + 1;
              
                break;
            }else{

             encBuffer[i] = encrypt(buffer[i], args->you_e, args->you_c);
            // write(args->connfd, &encBuffer, sizeof(encBuffer));
             
              i++;
            }
        }

        printf("String %s encrypted as (using server public key:%d, %d)", buffer, args->you_e, args->you_c);
        for(i = 0;  i < strlen(buffer); i++) {
            printf("%d ", encBuffer[i]);
            encBuffer[i] = htonl(encBuffer[i]);
        }
        printf("\n");
        printf("bufferSize = %d\n", bufferSize);
        int n = send(args->connfd, encBuffer, strlen(buffer) * sizeof(int) , 0);
        printf("write %d out\n", n);
        if(strcmp(buffer, "quit") == 0) {
            return;
 
        }
	 bzero(buffer, 256);
    }

}




int main(int argc, char** argv) {
	int n;
	int fd;
        srand(time(NULL)); 
        struct Arguments args;
	pthread_t readThread, writeThread;
        int a, b; //the two prime numbers
	if(argc < 5) {
	   printf("Usage: %s port\n", argv[0]);
	   exit(0);
	}
	
        a = atoi(argv[3]);
        b = atoi(argv[4]); 
        generatePrimeNumbers(&a, &b);
        printf("a = %d, b= %d\n", a , b);
        /*e,c for encrypt, d,c for decrypt*/
        int c = a * b; 
        int t = totient(a*b);
        int e = coprime(t);  
        int d = mod_inverse(e, (a-1) * (b-1)); 

	fd = connectTo(argv[2], atoi(argv[1]));
	

        args.connfd = fd;
        args.my_e  = e;
        args.my_c = c;
        args.my_d = d; 
        sendKeys(&args);
        receiveKeys(&args);

        pthread_create(&readThread, NULL, readData, &args);   
        pthread_create(&writeThread, NULL, writeData, &args);
	pthread_join(readThread, NULL);
        pthread_cancel(writeThread);
       
	
	
	close(fd);
}
