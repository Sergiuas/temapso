#include "utils.h"

int mode_to_flags(const char *mode)
{
	int ret = 0, m = 0, o = 0;

	if (strlen(mode) > 2 || strlen(mode) < 1) {
		perror("prob1 \n");
		errno = EINVAL;
		return (-1);
	}

	switch (*mode) {
	case 'r':
		m = O_RDONLY;
		o = 0;
		break;

	case 'w':
		m = O_WRONLY;
		o = O_CREAT | O_TRUNC;
		break;

	case 'a':
		m = O_WRONLY;
		o = O_CREAT | O_APPEND;
		break;

	default:
		errno = EINVAL;
		return (-1);
	}

	if (strlen(mode) == 2) {
		switch (mode[1]) {
		case '+':
			m = O_RDWR;
			break;
		default:
			perror("prob2\n");
			errno = EINVAL;
			return (-1);
		}
	}

	ret = m | o;
	return ret;
}
