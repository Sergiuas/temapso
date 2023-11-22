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



int mode_to_flags(const char *mode, int *optr);

#endif
