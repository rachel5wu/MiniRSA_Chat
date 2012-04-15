#include "mysocket.h"
#include <pthread.h>

int hasReturned = 0;
// rough function to convert lower to uppercase, and upper to lowercase
void mutate(char* buffer, int n) {
    int i;
    for(i = 0; i < n; i++) {
        if(buffer[i] > 0 && buffer[i] < 97) {
            buffer[i]+=32;
	} else {
	    buffer[i]-=32;
	}
    }
}

void *readData(void *fd) {
    int *connfd = (int *)fd;
    char buffer[256];
    bzero(buffer, 256);
    while(1) {
        read(*connfd, buffer, sizeof(buffer));
        puts(buffer);
        if(strcmp(buffer, "quit") == 0) {
        	write(*connfd, buffer, sizeof(buffer));
            return;    
        }
        bzero(buffer, 256);
    }
}

void *writeData(void *fd) {
    int *connfd = (int *)fd;
    char buffer[256];
    bzero(buffer, 256);
    int i = 0;
    while(1) {
        while(1) {
            buffer[i] = getchar();
            if( buffer[i] == '\n') {
                buffer[i] = '\0';
                i = 0;
                break;
            }else{
                i++;
            }
        }
        write(*connfd, buffer,strlen(buffer));
        if(strcmp(buffer, "quit") == 0) {return;}
        bzero(buffer, 256);
    }
}


int main(int argc, char** argv) {
    int srvfd, connfd;
    char buffer[256];
    int n;
    pthread_t readThread, writeThread;
	
    if(argc < 2) {
    	printf("Usage: %s port\n", argv[0]);
    	exit(0);
    }
    
    srvfd = makeListener(atoi(argv[1]));
    connfd = listenFor(srvfd);
    pthread_create(&readThread, NULL, readData, &connfd);
    pthread_create(&writeThread, NULL, writeData, &connfd);
    pthread_join(readThread, NULL);
    pthread_cancel(writeThread);
    close(connfd);
    close(srvfd);


}
