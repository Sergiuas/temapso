#include <stdio.h>
#include <fcntl.h>	
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PERMSIZE 9

int main(int argc, char **argv, char **envp){   

     if (argc != 3) {
        printf("Incorrect number of arguments");
        exit(-1);
    }

    int result=access(argv[1], F_OK);
    
    if (result == 0) {
        printf("Existing file");
        exit(-1);
    }

    if (strlen(argv[2])!=PERMSIZE){
        printf("Incorrect perm format");
        exit(-1);
    }

    for (int i=0;i<PERMSIZE;i++){
        if (i%3==0 && !strchr("-r", argv[2][i])){
            printf("Incorrect perm format2");
            exit(-1);
        }

        if (i%3==1 && !strchr("-w", argv[2][i])){
            printf("Incorrect perm format2");
            exit(-1);
        }

         if (i%3==2 && !strchr("-x", argv[2][i])){
            printf("Incorrect perm format2");
            exit(-1);
        }
    }




    int perm = 0;
    for (int i = 0; i < 9; i++) {
        if (argv[2][i] == 'r') {
            perm |= (i/3==0) ? S_IRUSR  : ((i/3==1) ? S_IRGRP : S_IROTH);
        } else if (argv[2][i] == 'w') {
            perm |= (i/3==0) ? S_IWUSR : ((i/3==1) ? S_IWGRP : S_IWOTH);
        } else if (argv[2][i] == 'x') {
            perm |= (i/3==0) ? S_IXUSR : ((i/3==1) ? S_IXGRP : S_IXOTH);
        }
    }
    int oldval = umask(0);
    int fd = open(argv[1], O_WRONLY | O_CREAT, perm);
        if (fd < 0) {
        perror("Error opening file 1");
        exit(-1);
    }
    umask(oldval);
    close(fd);

    return 0;
}
