#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 20

int is_symlink(const char *filename);

int main(int argc, char **argv, char **envp){   

    if (argc != 2) {
        printf("Incorrect numbers of arguments");
        exit(-1);
    }
    
    if (is_symlink(argv[1])==0){

        char buffer[SIZE+1];
        int fd = open(argv[1], O_RDONLY);

        if (fd < 0) {
        perror("Error operning file");
        exit(-1);
        }
        
        int rc = lseek(fd, -SIZE, SEEK_END);
        if (rc < 0) {
            perror("Error seeking to the end of the file");
            close(fd);
            exit(-1);
        }

        int bytesRead = read(fd, buffer, SIZE);
         if (bytesRead < 0) {
            perror("Error reading from file");
            close(fd);
            exit(-1);
        }

        buffer[bytesRead] = '\0';
        printf("The last %d bytes: \n%s\n", bytesRead, buffer);

        rc = close(fd);

    } else {
        printf("symbolic link\n");
        exit(1);
    }

    return 0;
}

int is_symlink(const char *filename){
    struct stat p_statbuf;

    if (lstat(filename, &p_statbuf) < 0) {  
        perror("calling stat()");
        exit(1);  
    }

    if (S_ISLNK(p_statbuf.st_mode) == 1) {
        return 1;
    } else {
        return 0;
    }
}