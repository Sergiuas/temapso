#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv, char** envp){
    
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc!=2){
        printf("Incorrect number of arguments");
        exit(-1);
    }

    int number = atoi(argv[1]);

    int index=0;
    int pid;
	
    for (int i=0;i<number;i++){
        pid = fork();

        if (pid == 0){
            index = i+1;
            printf("<pid:%d>: Executing the %dnd process\n", getpid(), index);
            break;
        }
    }

    return 0;
}