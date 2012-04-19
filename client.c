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
    bzero(buffer, 256);
    int num=0, i;
  
    while(args->run) {
        printf("\nFriend: ");
        num =  read(args->connfd, encBuffer, sizeof(encBuffer));
        for(i=0; i< num/8; i++){
            printf("%ld ", encBuffer[i]);
        }
        printf("\n");
        for(i=0; i< num/8; i++){
            buffer[i] = decrypt(encBuffer[i], args->my_d, args->my_c);
            printf("%c", buffer[i]);
        }
        buffer[i] = '\0';
        if(strcmp(buffer, "quit") == 0) {
	    write(args->connfd, buffer, sizeof(buffer));
            args->run = 0;
            break;    
        }
        bzero(buffer, 256);
        bzero(encBuffer, 256);
    }
    pthread_exit(0);
}

void *writeData(void *arguments) {
    struct Arguments *args = (struct Arguments *)arguments;

    char buffer[256];
    char temp;
    long encBuffer[256];
    int num = 0;
    int i = 0;
    while(args->run) {
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
      
        for(i =  0; i < strlen(buffer); i++) {
            encBuffer[i] = encrypt(buffer[i], args->you_e, args->you_c);
            printf("%ld ", encBuffer[i]);
        }
        encBuffer[i] = '\0';
        send(args->connfd, encBuffer, (strlen(buffer) + 1) * sizeof(long), 0);
        if(strcmp(buffer, "quit")==0){args->run = 0; break;}
        bzero(buffer, sizeof(buffer));
        bzero(encBuffer, sizeof(encBuffer));
    } 
    pthread_exit(0);
}

int main(int argc, char** argv) {
	int n;
	int fd;
        long dec, enc; 
        srand(time(NULL)); 
        struct Arguments args;
	pthread_t readThread, writeThread;
        long a, b; //the two prime numbers
	if(argc < 5) {
	   printf("file portNum hostName prime1 prime2\n");
	   exit(0);
	}
        a = atoi(argv[3]) + 6;
        b = atoi(argv[4]) + 6; 
        generatePrimeNumbers(&a, &b);
        generateKeys(a, b, &args.my_e, &args.my_d, &args.my_c);
        args.run = 1;  
        printf("inverse = %d", (args.my_e * args.my_d)  % ((a - 1)*(b - 1)));
	fd = connectTo(argv[2], atoi(argv[1]));
        args.connfd = fd;
        
        sendKeys(&args);
        receiveKeys(&args);
        printKeys(args);
        
        pthread_create(&readThread, NULL, readData, &args);
        pthread_create(&writeThread, NULL, writeData, &args);
        pthread_join(readThread, NULL);
        pthread_cancel(writeThread);
	close(fd);
}

