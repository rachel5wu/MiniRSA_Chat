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


int hasReturned = 0;

void sendKeys( struct Arguments *args){
    write(args->connfd, &args->my_e, sizeof(int));
    write(args->connfd, &args->my_c, sizeof(int));
    printf("My public key: %d %d\n", args->my_e, args->my_c);
    printf("My private key: %d, %d\n", args->my_d, args->my_c);
}


void receiveKeys(struct Arguments *args) {
    recv(args->connfd, &args->you_e, sizeof(int), 0);
    recv(args->connfd, &args->you_c, sizeof(int), 0);
    printf("Friends' public key : %d %d\n", args->you_e, args->you_c);
}

void *readData(void *fd) {
    struct Arguments *args = (struct Arguments *)fd;
    int bufferSize = 0;
    int encBuffer[256];
    char buffer[256];  
    bzero(buffer, 256);
     
    bzero(encBuffer, 256);
    int i = 0;
    
    while(1) {
    
        bufferSize = recv(args->connfd, encBuffer, sizeof(encBuffer), 0);
        bufferSize /= 4;
        printf("Server recieved data\n"); 
        printf("bufferSize: %d\n", bufferSize);  
        
        for (i = 0; i < bufferSize; i++) {
                 encBuffer[i] = ntohl(encBuffer[i]);
                 printf("%d\n", encBuffer[i]);
        }
        printf("BEGIN DECRYPT!!!!!!d=%d, c=%d", args->my_d, args->my_c);
        for(i = 0; i < bufferSize; i++) {
            buffer[i] = decrypt(encBuffer[i], args->my_d, args->my_c );
            printf("buffer[i] = %c\n", buffer[i]);
        
        } 

        if(strcmp(buffer, "quit") == 0) {
	    write(args->connfd, buffer, sizeof(buffer));
            return;    
        }
         bzero(buffer, 256);
      
    
    }
}

void *writeData(void *fd) {
    struct Arguments *args = (struct Arguments *)fd;
    
    char buffer[256];
    bzero(buffer, 256);
    int i = 0;
    while(1) {
        while(1) {
        if((buffer[i] = getchar()) == '\n') {
            buffer[i] = '\0';
            i = 0;
            break;
        }
        else {
            i++;
        }
        }
       
        write(args->connfd, buffer,strlen(buffer));
        if(strcmp(buffer, "quit") == 0) {
          
           return;
       }
        bzero(buffer, 256);
      
    }
}


int main(int argc, char** argv) {
        srand(time(NULL)); 
        pthread_t readThread, writeThread;
        struct Arguments args;
	int srvfd, connfd;
	char buffer[256];
	int a, b; //the two prime numbers
	
	if(argc < 4) {
	   printf("Usage: %s port\n", argv[0]);
	   exit(0);
	}
        a = atoi(argv[2]);
        b = atoi(argv[3]); 
        generatePrimeNumbers(&a, &b);
        printf("a = %d, b= %d\n", a , b);
        /*e,c for encrypt, d,c for decrypt*/
        int c = a * b; 
        int t = totient(a*b);
        int e = coprime(t);  
        int d = mod_inverse(e, (a - 1) * (b - 1)); 
        printf("c = %d t = %d e = %d d = %d\n", c,t,e,d);
	

	srvfd = makeListener(atoi(argv[1]));
	connfd = listenFor(srvfd);
        args.connfd = connfd;
        args.my_e  = e;
        args.my_c = c;
        args.my_d = d; 
        sendKeys(&args);
        receiveKeys(&args);
        printf("e%d c%d d%d\n", args.my_e, args.my_c, args.my_d);
        pthread_create(&readThread, NULL, readData, &args);
        pthread_create(&writeThread, NULL, writeData, &args);
        pthread_join(readThread, NULL);
        pthread_cancel(writeThread);
	close(connfd);	
	close(srvfd);
}
