#include "readWriteData.h"

int main(int argc, char** argv) {
        srand(time(NULL)); 
        pthread_t readThread, writeThread;
        struct Arguments args;
	int srvfd, connfd;
	char buffer[256];
	long a, b; // two prime numbers
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

        pthread_create(&readThread, NULL, readData, &args);
        pthread_create(&writeThread, NULL, writeData, &args);
        pthread_join(readThread, NULL);
        pthread_cancel(writeThread);
        close(connfd);
        close(srvfd);
}
 
