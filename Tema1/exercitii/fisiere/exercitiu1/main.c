#include <fcntl.h>
#include <unistd.h>

#define SIZE 16

int main(int argc, char** argv, char**envp) {
    // Deschideți fișierul pentru citire
    int fd = open("file", O_RDONLY);
    
    if (fd == -1) {
        return 1;
    }

    char buffer[SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytesRead);
    }

    close(fd);

    return 0;
}
