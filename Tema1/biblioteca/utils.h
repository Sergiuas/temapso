#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


#define BUFFER_SIZE 4096
#define PERMISSIONS 0644

#define READ 1
#define WRITE 2


struct _so_file{

    short	_flags;		/* (*) flags, below; this FILE is free if 0 */
	short	_file;		/* (*) fileno, if Unix descriptor, else -1 */


    char* _buf;
    int offset;

    pid_t _pid;

    int _feof; /* Set when the pointer reaches the end of the file */
    int _ferror;

    int _operation;
    
    long _file_pointer_pos;
};


int mode_to_flags(const char *mode);

#endif
