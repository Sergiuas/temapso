#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define PERMISSIONS 0644

#define READ 1
#define WRITE 2

#define SO_TRUE 1
#define SO_FALSE 0

#define READ_END 0
#define WRITE_END 1
#define PIPE_BUFFER_SIZE 32

struct _so_file {

	int _flags; /* (*) flags, below; this FILE is free if 0 */
	int _file;  /* (*) fileno, if Unix descriptor, else -1 */

	char *_buf_base;
	char *_buf_end;

	int offset;
	int actualSize;

	pid_t _pid;

	int _feof; /* Set when the pointer reaches the end of the file */
	int _ferror;

	int _operation;

	long _file_pointer_pos;
};

int mode_to_flags(const char *mode);

#endif
