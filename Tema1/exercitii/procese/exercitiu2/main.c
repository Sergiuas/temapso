#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define READ_END 0
#define WRITE_END 1
#define BUFSIZE 32

int myatoi(const char *s, int *value)
{

    if ( s != NULL && *s != '\0' && value != NULL )
    {
        char *endptr = s;
        *value = (int)strtol(s, &endptr, 10);
        if ( *endptr == '\0' )
        {
            return 1;
        }
    }
    return 0; /* failed to convert string to integer */
}

void search_number(int start, int end, int value, int* numbers, int pipe_fd[2]) {
    close(pipe_fd[READ_END]);

    //dup2(pipe_fd[WRITE_END], STDOUT_FILENO);



    for (int i = start; i < end; i++) {
        if (numbers[i] == value) {
           // char buffer[BUFSIZE];
           // sprintf(buffer, "%d ", i);
            write(pipe_fd[WRITE_END], &i, sizeof(int));

        }
    }
        close(pipe_fd[WRITE_END]);
}

int main(int argc, char **argv, char**envp){

    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc!=4){
        printf("Incorrect number of arguments");
        exit(-1);
    }

    char* filename = (char*)malloc((strlen(argv[1])+1)*sizeof(char));
    strcpy(filename, argv[1]);

    int procNumber, value, result;

    result = myatoi(argv[2], &procNumber);
    if (result==0){
        printf("Invalid integer format ");
        exit(-1);
    }

    result = myatoi(argv[3], &value);
    if (result==0){
        printf("Invalid integer format ");
        exit(-1);
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int nrNumbers;
    fscanf(file, "%d", &nrNumbers);

    int *numbers = malloc(nrNumbers * sizeof(int));
    for (int i = 0; i < nrNumbers; i++) {
        fscanf(file, "%d", &numbers[i]);
    }
    fclose(file);

    int *chunkSizes = malloc(procNumber * sizeof(int));
    int q = nrNumbers / procNumber;
    int r = nrNumbers % procNumber;
    for (int i=0;i<procNumber;i++){
        chunkSizes[i]=q;
    }

    for(int i=0;i<r;i++){
         chunkSizes[i]++;
    }

    int pipe_fd[procNumber][2];
    pid_t pid;
    int start = 0;
    int end = 0;

    for (int i = 0; i < procNumber; ++i) {

        if (pipe(pipe_fd[i]) == -1) {
            perror("Error creating pipe");
            exit(-1);
        }
       
        start = end;
        end += chunkSizes[i];

        pid_t pid = fork();

        if (pid == 0) { 
            search_number(start, end, value, numbers, pipe_fd[i]);
            exit(1);
        }
    }


    int status=0;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0);



    for (int i=0;i<procNumber;i++){

        close(pipe_fd[i][WRITE_END]);
           
        int res=-1;

       // char buffer[BUFSIZE];
        while (read(pipe_fd[i][READ_END], &res, sizeof(int)) > 0) {
        printf("Indexul %d si procesul %d\n", res, i);
        }   
        close(pipe_fd[i][READ_END]);
    }
    


    return 0;
}