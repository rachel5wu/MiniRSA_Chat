#include "mysocket.h"
#include <pthread.h>

void *readData(void *connfd) {
    int *fd = (int *)connfd;
    char buffer[256];
    while(1) {
        read(*fd, buffer, sizeof(buffer));
        puts(buffer);
        if (strcmp("quit", buffer) == 0) {
            write(*fd, buffer, strlen(buffer));
            return;
        }
        bzero(buffer, 256);
    }
}

void *writeData(void *connfd) {
    int *fd = (int *)connfd;
    char buffer[256];
    int i = 0;
    while(1) {
        while(1) {
            buffer[i] = getchar();
            if(buffer[i] == '\n') {
                buffer[i] = '\0';
                i = 0;
                break;
            }else {
                i++;
            }
        }
        write(*fd, buffer, strlen(buffer));
        if(strcmp(buffer, "quit") == 0) {
            return;
        }
	    bzero(buffer, 256);
    }
}


int main(int argc, char** argv) {
	int n;
	int fd;
	pthread_t readThread, writeThread;
	if(argc < 3) {
	   printf("Usage: %s port\n", argv[0]);
	   exit(0);
	}
	
	fd = connectTo(argv[2], atoi(argv[1]));
	
    pthread_create(&readThread, NULL, readData, &fd);   
    pthread_create(&writeThread, NULL, writeData, &fd);
	pthread_join(readThread, NULL);
    pthread_cancel(writeThread);
    
	close(fd);
}
