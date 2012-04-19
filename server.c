#include "mysocket.h"
#include <pthread.h>
#include "RSALIBRARY.h"


void sendKeys( struct Arguments *args){
    send(args->connfd, &args->my_e, sizeof(long), 0);
    send(args->connfd, &args->my_c, sizeof(long), 0);
    printf("My public key: %d %d\n", args->my_e, args->my_c);
    printf("My private key: %d, %d\n", args->my_d, args->my_c);
}


void receiveKeys(struct Arguments *args) {
    recv(args->connfd, &args->you_e, sizeof(long), 0);
    recv(args->connfd, &args->you_c, sizeof(long), 0);
    printf("Friends' public key : %d %d\n", args->you_e, args->you_c);
}

void *readData(void *arguments) {
    struct Arguments *args = (struct Arguments *)arguments;
    
    char buffer[256];
    long encBuffer[256];
    int i = 0;
    int numOfBytes;
   
    while(args->run) {
        printf("\nFriend: ");
        numOfBytes = recv(args->connfd, encBuffer, sizeof(encBuffer), 0);
        for(i = 0; i< numOfBytes/8; i++){
           printf("%ld ", encBuffer[i]);
        }
        printf("\n");
        for(i = 0; i < numOfBytes / 8; i++) {
            buffer[i] = decrypt(encBuffer[i], args->my_d, args->my_c);
            printf("%c",  buffer[i]);
        }
        buffer[i] = '\0';
        if(strcmp(buffer, "quit") == 0) {
	    write(args->connfd, buffer, sizeof(buffer));
            args->run = 0;
            return;
        }
        bzero(buffer, sizeof(buffer));
        bzero(encBuffer, sizeof(encBuffer));
    }
    pthread_exit(0);
}

void *writeData(void *arguments) {
    struct Arguments *args = (struct Arguments *)arguments;
    
    char buffer[256];
    long encBuffer[256];
    int i = 0, num = 0;

    while(1) {
        i = 0;
        printf("\nMe: ");
        while(1) {
            buffer[i] = getchar();
            if(buffer[i] == '\n') {
                buffer[i] = '\0';
                break;
            }
            i++;
        }
        for(i=0; i<strlen(buffer); i++){
           encBuffer[i] = encrypt(buffer[i], args->you_e, args->you_c);
           printf("%ld ", encBuffer[i]);
        }
        encBuffer[i] = '\0';
        send(args->connfd, encBuffer,(strlen(buffer)+1)*sizeof(long), 0);
        if(strcmp(buffer, "quit") == 0) { args->run = 0;return;}
        bzero(buffer, sizeof(buffer));
        bzero(encBuffer, sizeof(encBuffer));
    }
    pthread_exit(0);
}

int main(int argc, char** argv) {
        srand(time(NULL)); 
        pthread_t readThread, writeThread;
        struct Arguments args;
	int srvfd, connfd;
	char buffer[256];
	long a, b; //the two prime numbers
	char base[256];
        int i;
        long enc, dec;
	if(argc < 4) {
	   printf("file portNum prime1 prime2\n");
	   exit(0);
	}
        a = atoi(argv[2]) + 6;
        b = atoi(argv[3]) + 6; 
        generatePrimeNumbers(&a, &b);
        generateKeys(a, b, &args.my_e, &args.my_d, &args.my_c);
        args.run = 1;
	srvfd = makeListener(atoi(argv[1]));
	connfd = listenFor(srvfd);
        args.connfd = connfd;
         
        sendKeys(&args);
        receiveKeys(&args);
        printKeys(args);

        pthread_create(&readThread, NULL, readData, &args);
        pthread_create(&writeThread, NULL, writeData, &args);
        pthread_join(readThread, NULL);
        pthread_cancel(writeThread);
        close(connfd);
        close(srvfd);
}
 
