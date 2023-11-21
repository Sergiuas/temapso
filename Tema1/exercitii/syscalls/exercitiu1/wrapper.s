.intel_syntax noprefix
	.text
	.section .rodata

    .text
	.globl open_wrapper
    # Wrapper for open
    # int open(const char *pathname, int flags, mode_t mode);
    open_wrapper:
        mov rax, 2           # pathname
        mov rsi, rsi         # flags
        mov rdx, rdx         # mode
        syscall
        ret

    .globl read_wrapper
    # Wrapper for read
    # ssize_t read(int fd, void *buf, size_t count);
    read_wrapper:
        mov rax, 0           # syscall number for read
        mov rdi, rdi         # fd
        mov rsi, rsi         # buf
        mov rdx, rdx         # count
        syscall
        ret

.globl write_wrapper
    # Wrapper for write
    # ssize_t write(int fd, const void *buf, size_t count);
    write_wrapper:
        mov rax, 1           # syscall number for write
        mov rdi, rdi         # fd
        mov rsi, rsi         # buf
        mov rdx, rdx         # count
        syscall
        ret

.globl close_wrapper
    # Wrapper for close
    # int close(int fd);
    close_wrapper:
        mov rax, 3           # syscall number for close
        mov rdi, rdi         # fd
        syscall
        ret
