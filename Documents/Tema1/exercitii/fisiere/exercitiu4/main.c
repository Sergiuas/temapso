#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>    
 #include <stdio.h>
#include <sys/stat.h>

#define SIZE 10

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
 
    off_t dim = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if (dim>10){
       // lseek(fd, 10, SEEK_SET);
        ftruncate(fd, 10);
    }

    char buffer[SIZE+1];
    const char caractere[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i=0; i<SIZE;i++){
         buffer[i] = caractere[rand() % (sizeof(caractere) - 1)];
    }

    buffer[SIZE]='\0';
    lseek(fd, 0, SEEK_END);
    write(fd, buffer, sizeof(buffer));
    

    close(fd);
}

