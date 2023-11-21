#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 16

 int open_wrapper(const char *pathname, int flags, mode_t mode);
 ssize_t read_wrapper(int fd, void *buf, size_t count);
 ssize_t write_wrapper(int fd, const void *buf, size_t count);
 int close_wrapper(int fd);

int main(int argc, char** argv, char** envp) {
    
    int fd = open_wrapper(argv[1], O_RDONLY, 0);

    if (fd == -1) {
        perror("Eroare la deschiderea fișierului");
        exit(-1);
    }

    char buffer[SIZE];
    ssize_t bytesRead = read_wrapper(fd, buffer, sizeof(buffer));

    if (bytesRead == -1) {
        perror("Eroare la citirea din fișier");
        close_wrapper(fd);
        exit(-1);
    }

    write_wrapper(STDOUT_FILENO, buffer, bytesRead);

    close_wrapper(fd);

    return 0;
}
