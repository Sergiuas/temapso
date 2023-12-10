#include "so_stdio.h"
#include "utils.h"

int mode_to_flags(const char *mode);

SO_FILE *so_fopen(const char *pathname, const char *mode)
{

	SO_FILE *fp;

	int flags = mode_to_flags(mode);
	int fd = -1;

	if (flags == -1) {
		perror("permc");
		return NULL;
	}

	fd = open(pathname, flags, PERMISSIONS);
	if (fd == -1) {
		perror("fisier");
		return NULL;
	}

	fp = (SO_FILE *)malloc(sizeof(SO_FILE));
	if (fp == NULL) {
		perror("malloc fp");
		free(fp);
		return NULL;
	}

	fp->_buf_base = (char *)malloc(BUFFER_SIZE * sizeof(char));
	if (fp->_buf_base == NULL) {
		perror("malloc buffer");
		close(fd);
		free(fp->_buf_base);
		free(fp);
		return NULL;
	}

	fp->_flags = flags;
	fp->offset = 0;
	fp->_file = fd;
	fp->_operation = 0;
	fp->_feof = SO_FALSE;
	fp->_ferror = 0;
	fp->_file_pointer_pos = 0;
	fp->_pid = -1;
	fp->_buf_end = fp->_buf_base;
	fp->actualSize = 0;

	memset(fp->_buf_base, 0, BUFFER_SIZE);

	return fp;
}

int so_fclose(SO_FILE *stream)
{

	if (stream == NULL)
		return SO_EOF;

	if (stream->actualSize != 0 && stream->_operation == WRITE) {
		int flu = so_fflush(stream); // de gandit un pic si pentru stdin

		if (flu == SO_EOF) {
			free(stream->_buf_base);
			free(stream);
			return SO_EOF;
		}
	}

	int rc = close(stream->_file);

	if (rc == SO_EOF) {
		stream->_ferror = SO_TRUE;
		free(stream->_buf_base);
		free(stream);
		return SO_EOF;
	}

	free(stream->_buf_base);
	free(stream);

	return 0;
}

int so_fileno(SO_FILE *stream)
{
	if (stream == NULL)
		return SO_EOF;

	return stream->_file;
}

int so_fflush(SO_FILE *stream)
{

	if (stream->_operation == READ)
		return 1;

	if (stream == NULL)
		return SO_EOF;

	if (!(stream->_flags & O_WRONLY || stream->_flags & O_RDWR))
		return SO_EOF;

	// if (stream->_feof)
	//  return SO_EOF;
	// for (int i=0;i<stream->offset;i++){
	//  int c = (int)stream->_buf_base[i];
	// printf("%i - %d\n", i, c);
	//  }
	int bytesWrote = write(stream->_file, stream->_buf_base, stream->offset);
	int bytesTemp = 0;

	if (bytesWrote == -1) {
		stream->_ferror = SO_TRUE;
		return SO_EOF;
	}

	while (bytesWrote < stream->offset) {
		bytesTemp = write(stream->_file, stream->_buf_base + bytesWrote, stream->offset - bytesWrote);
		bytesWrote = bytesWrote + bytesTemp;

		if (bytesTemp == -1) {
			stream->_ferror = SO_TRUE;
			return SO_EOF;
		}
	}

	stream->_file_pointer_pos += bytesWrote; // de gandit un pica, ori il maresc aici si nu la fputc, la write ar trebui;
	stream->offset = 0;
	stream->actualSize = 0;
	stream->_operation = WRITE;

	memset(stream->_buf_base, 0, BUFFER_SIZE);

	return 0;
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{

	if (stream == NULL)
		return SO_EOF;

	if (stream->_file < 0)
		return SO_EOF;

	if (!(whence == SEEK_CUR || whence == SEEK_END || whence == SEEK_SET))
		return SO_EOF;

	if (stream->_operation == WRITE)
		so_fflush(stream);

	memset(stream->_buf_base, 0, BUFFER_SIZE);

	stream->offset = 0;
	stream->actualSize = 0;

	int off = lseek(stream->_file, offset, whence);

	stream->_file_pointer_pos = off; //

	// in caz ca fac fseek dupa ce s-a facut feof il fac false;
	return 0;
}

long so_ftell(SO_FILE *stream)
{
	if (stream == NULL)
		return SO_EOF;

	if (stream->actualSize != 0 && stream->_operation == WRITE)
		so_fflush(stream);

	return stream->_file_pointer_pos; // return lseek(..., 0, cur);
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{

	if (!(stream->_flags == O_RDONLY || stream->_flags & O_RDWR))
		return SO_EOF;

	if (stream == NULL)
		return SO_EOF;

	if (stream->_file < 0)
		return SO_EOF;

	if (stream->_feof)
		return SO_EOF;

	if ((stream->_flags & O_RDWR) && stream->_operation == WRITE && stream->actualSize != 0)
		so_fflush(stream);

	// if (stream->flags&O_APPEND){
	//     lseek(stream->_file, 0, SEEK_SET);
	//   }

	// if((stream->_flags & O_RDWR) && stream->_operation==WRITE)
	//   so_fflush(stream);

	ssize_t totalBytes = 0;
	ssize_t toRead = size * nmemb;
	int readRes = 0;

	while (totalBytes < toRead) { // && stream->_feof==SO_FALSE){// && (stream->_feof==SO_FALSE || stream->actualSize!=0)){//&& stream->_feof==SO_FALSE){

		readRes = so_fgetc(stream);

		if (stream->_ferror == SO_TRUE)
			break;
		if (stream->_feof == SO_TRUE)
			break;

		((char *)ptr)[totalBytes] = (char)readRes;
		totalBytes++;
	}

	if (stream->_ferror == SO_TRUE)
		return 0;

	return (int)totalBytes / size;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{

	if (!(stream->_flags & O_WRONLY || stream->_flags & O_RDWR))
		return 0;

	// if((stream->_flags & O_RDWR) && stream->_operation==READ)
	//   lseek(stream->_file, 0, SEEK_SET);

	// if (stream->offset&O_APPEND){
	//   lseek(stream->_file, 0, SEEK_END);
	//}

	ssize_t totalBytes = 0;
	ssize_t toWrite = size * nmemb;

	unsigned char *loader = (unsigned char *)ptr;
	int resWrite = 0;
	int nrMembers = 0;

	while (totalBytes < toWrite) {
		int c = loader[totalBytes];

		resWrite = so_fputc(c, stream);
		totalBytes++;

		// if(resWrite==SO_EOF){
		// perror("probl\n");
		// return SO_EOF;
		//}
	}
	return nmemb;
}

int so_fgetc(SO_FILE *stream)
{

	if (stream == NULL)
		return SO_EOF;

	if (stream->_feof == SO_TRUE)
		return SO_EOF;

	if (!(stream->_flags == O_RDONLY || stream->_flags & O_RDWR))
		return SO_EOF;

	if ((stream->_flags & O_RDWR) && stream->_operation == WRITE && stream->actualSize != 0)
		so_fflush(stream);

	if (stream->offset == stream->actualSize) {
		ssize_t bytesRead = 0;

		memset(stream->_buf_base, 0, BUFFER_SIZE);

		bytesRead = read(stream->_file, stream->_buf_base, BUFFER_SIZE);
		if (bytesRead == -1) {

			stream->_ferror = SO_TRUE;
			return SO_EOF;
		}

		stream->actualSize = bytesRead;
		stream->offset = 0;

		// int poz=lseek(stream->_file,0,SEEK_CUR);

		if (bytesRead == 0) {
			stream->_feof = SO_TRUE;
			return SO_EOF;
		}
	}

	int result = stream->_buf_base[stream->offset];

	stream->offset++;
	stream->_file_pointer_pos++;
	stream->_operation = READ;

	return result;
}

int so_fputc(int c, SO_FILE *stream)
{

	if (stream == NULL)
		return SO_EOF;

	if (!(stream->_flags & O_WRONLY || stream->_flags & O_RDWR))
		return SO_EOF;

	// if (stream->_feof){
	//   return SO_EOF;
	//}

	if (stream->offset == BUFFER_SIZE) {

		int rv = so_fflush(stream);

		if (rv == SO_EOF)
			return SO_EOF;
	}

	stream->_buf_base[stream->offset] = (unsigned char)c;
	stream->offset++;
	stream->actualSize++;
	stream->_operation = WRITE;

	return c;
}

int so_feof(SO_FILE *stream)
{

	if (stream == NULL)
		return SO_EOF;

	if (stream->_feof)
		return SO_EOF;

	return 0;
}

int so_ferror(SO_FILE *stream) { return stream->_ferror; }

SO_FILE *so_popen(const char *command, const char *type)
{

	SO_FILE *fp = NULL;

	if (strcmp(type, "r") != 0 && strcmp(type, "w") != 0)
		return NULL;

	int flags = mode_to_flags(type);

	int ret = -1;
	int pipe_fd[2];

	ret = pipe(pipe_fd);

	if (ret < 0) {
		perror("error on creating pipe: ");
		return NULL;
	}

	int pid;

	pid = fork();
	if (pid < 0) {
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		// free(fp->_buf_base);
		// free(fp);
		return NULL;
	}

	if (pid == 0) {

		if (strcmp(type, "r") == 0) {
			close(pipe_fd[READ_END]);
			dup2(pipe_fd[WRITE_END], STDOUT_FILENO);
			close(pipe_fd[WRITE_END]);
		} else {
			close(pipe_fd[WRITE_END]);
			dup2(pipe_fd[READ_END], STDIN_FILENO);
			close(pipe_fd[READ_END]);
		}

		execlp("sh", "sh", "-c", command, NULL);
		exit(1);
	}

	if (pid > 0) {

		fp = (SO_FILE *)malloc(sizeof(SO_FILE));
		if (fp == NULL) {
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			perror("malloc fp");
			//  free(fp);
			return NULL;
		}

		fp->_buf_base = (char *)malloc(BUFFER_SIZE * sizeof(char));
		if (fp->_buf_base == NULL) {
			perror("malloc buffer");
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			// free(fp->_buf_base);
			free(fp);
			return NULL;
		}

		fp->_flags = flags;
		fp->offset = 0;
		fp->_operation = 0;
		fp->_feof = SO_FALSE;
		fp->_ferror = 0;
		fp->_file_pointer_pos = 0;
		fp->_pid = pid;
		fp->_buf_end = fp->_buf_base;
		fp->actualSize = 0;

		memset(fp->_buf_base, 0, BUFFER_SIZE);

		if (strcmp(type, "r") == 0) {
			close(pipe_fd[WRITE_END]);
			fp->_file = pipe_fd[READ_END];
		} else {
			close(pipe_fd[READ_END]);
			fp->_file = pipe_fd[WRITE_END];
		}

		return fp;
	}

	// close(pipe_fd[0]);
	// close(pipe_fd[1]);
	// free(fp->_buf_base);
	// free(fp);
	return NULL;
}

int so_pclose(SO_FILE *stream)
{

	if (stream == NULL)
		return SO_EOF;

	if (stream->_pid < 0)
		return SO_EOF;

	int pid = stream->_pid;

	if (so_fclose(stream) < 0)
		return SO_EOF;

	int status = 0;

	if (waitpid(pid, &status, 0) < 0)
		return SO_EOF;

	return status;
}
