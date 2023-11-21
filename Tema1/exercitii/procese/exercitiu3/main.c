#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <asm-generic/signal-defs.h>

#define PIPE_NAME "my_pipe"

volatile sig_atomic_t signal_received = 0;

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

void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        signal_received = 1; 
       
    }
}

void work(int procNr){

    while(1){
        int pipe_fd = open(PIPE_NAME, O_RDONLY);
        int nr;
        int ret = read(pipe_fd, &nr, sizeof(int));

        if(ret!=0){
            if (nr!=0){
                printf("Process %d, pid: %d child working.\n",procNr, getpid());
            }
            else {
                close(pipe_fd);
                return;
            }
        }

        close(pipe_fd);
    }
}

int main(int argc, char** argv, char**envp){

     setvbuf(stdout, NULL, _IONBF, 0);

    if (argc!=2){
        printf("Incorrect number of arguments");
        exit(-1);
    }

    int ret;
	ret = mkfifo(PIPE_NAME, 0644);
	if (ret < 0) {
		perror("error on create pipe: ");
	}

    int procNumber;
    int result = myatoi(argv[1], &procNumber);
     if (result==0){
        printf("Invalid integer format ");
        exit(-1);
    }


    pid_t pid;
    for (int i=0;i<procNumber;i++){

        
        pid = fork();
        if (pid == 0){
            printf("Process %d child started.\n", getpid());
            work(i);
            return 0;
        }

    }

    struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
 
	printf("Process %d parent started.\n", getpid());
	sa.sa_flags = SA_RESETHAND;
	sa.sa_handler = signal_handler;
	sigaction(SIGUSR1, &sa, NULL);
    sigset_t mask;
    if (sigfillset(&mask) != 0) {
        perror("Error in sigfillset");
        return 1;
    }

    if (sigdelset(&mask, SIGUSR1) != 0) {
        perror("Error in sigdelset");
        return 1;
    }

    if (sigprocmask(SIG_SETMASK, &mask, NULL) != 0) {
        perror("Error in sigprocmask");
        return 1;
    }

    while(signal_received==0){
        //printf("Send a command number: ");
        int nr;
        scanf("%d", &nr);
        int pipe_fd = open(PIPE_NAME, O_WRONLY);
        for(int i=0;i<procNumber;i++)
            write(pipe_fd, &nr, sizeof(int));
        fflush(stdin);
        close(pipe_fd);
        
        if (nr == 0){
            int rm = unlink(PIPE_NAME);
            if(rm<0){
                perror("Error unlinking the pipe");
                exit(-1);
            }
           
            return 0;
        }
    }

    int nr =0;
    int pipe_fd = open(PIPE_NAME, O_WRONLY);
    for(int i=0;i<procNumber;i++)
    write(pipe_fd, &nr, sizeof(int));
    close(pipe_fd);
    int rm = unlink(PIPE_NAME);
    if(rm<0){
        perror("Error unlinking the pipe");
        exit(-1);
    }

    return 0;
}