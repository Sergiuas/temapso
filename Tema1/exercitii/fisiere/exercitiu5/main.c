#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <asm-generic/fcntl.h>


#define SIZE 1024

int main(int argc, char** argv, char** envp) 
{
    
    if (argc!=2){
        printf("Incorrect number of arguments");
        exit(-1);
    }

    int fd;
 
    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd < 0) {
        perror("Error opening file 1");
        exit(-1);
    }

    char buffer[SIZE];
    int bytesRead;
    bytesRead = read(fd, buffer, SIZE);
    buffer[bytesRead] = '\0';

    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NOATIME);

    //lseek(fd, 0, SEEK_END);
    write(fd, buffer, bytesRead);

    close(fd);
    return 0;
}