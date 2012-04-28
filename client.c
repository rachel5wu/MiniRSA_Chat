
#include "readWriteData.h"

int main(int argc, char** argv) {
	int n, fd;
        long dec, enc; 
        struct Arguments args;
	pthread_t readThread, writeThread;
        long a, b; // two prime numbers
	if(argc < 5) {
	   printf("file portNum hostName prime1 prime2\n");
	   exit(0);
	}
        a = atoi(argv[3]) + 6;
        b = atoi(argv[4]) + 6; 
        generatePrimeNumbers(&a, &b);
        generateKeys(a, b, &args.my_e, &args.my_d, &args.my_c);
        args.run = 1;  
	args.who = 1;

        fd = connectTo(argv[2], atoi(argv[1]));
        args.connfd = fd;
        sendKeys(&args);
        receiveKeys(&args);
        
        pthread_create(&readThread, NULL, readData, &args);
        pthread_create(&writeThread, NULL, writeData, &args);
        pthread_join(readThread, NULL);
       // pthread_cancel(writeThread);
	close(fd);
        exit(0);
}

