#include "utils.h"
#include "so_stdio.h"
#include <stdio.h>

int mode_to_flags(const char* mode);


SO_FILE *so_fopen(const char *pathname, const char *mode)
{

    SO_FILE* fp = NULL;
    int flags, fd;

    if ((flags=mode_to_flags(mode))==0){
                perror("permc");
        return NULL;
    }

    fd = open(pathname, flags, PERMISSIONS);
    if (fd == -1){
        printf("%s path \n", pathname);
        perror("fisier");
        return NULL;
    }

    fp = (SO_FILE*)malloc(sizeof(SO_FILE));
    if (fp==NULL){
         perror("malloc");
        return NULL;
    }

    fp->_buf=(char*)malloc(BUFFER_SIZE*sizeof(char));
    if (fp->_buf==NULL){
        perror("malloc buffer");
        close(fd);
        free(fp);
        return NULL;
    }

    fp->offset=0;
    fp->_file=fd;
    fp->_operation=0;
    fp->_ferror=0;
    fp->_file_pointer_pos=0;

    return fp;
}

int so_fclose(SO_FILE *stream)
{
    return 0;
}

int so_fileno(SO_FILE *stream)
{
    return 0;
}

int so_fflush(SO_FILE *stream)
{
    return 0;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
    return 0;
}

long so_ftell(SO_FILE *stream)
{
    return 0;
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    return 0;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    return 0;
}

int so_fgetc(SO_FILE *stream)
{
    return 0;
}

int so_fputc(int c, SO_FILE *stream)
{
    return 0;
}

int so_feof(SO_FILE *stream)
{
    return 0;
}

int so_ferror(SO_FILE *stream)
{
    return 0;
}

SO_FILE *so_popen(const char *command, const char *type)
{
    return NULL;
}

int so_pclose(SO_FILE *stream)
{
    return 0;
}
