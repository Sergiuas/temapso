.intel_syntax noprefix
	.text
	.section .rodata

    .text
	.globl open_wrapper
    # Wrapper pentru open
    # int open(const char *pathname, int flags, mode_t mode);
    open_wrapper:
        mov rax, 2           
        mov rsi, rsi         
        mov rdx, rdx         
        syscall
        ret

    .globl read_wrapper
    # Wrapper pentru read
    # ssize_t read(int fd, void *buf, size_t count);
    read_wrapper:
        mov rax, 0           
        mov rdi, rdi         
        mov rsi, rsi         
        mov rdx, rdx         
        syscall
        ret

.globl write_wrapper
    # Wrapper pentru write
    # ssize_t write(int fd, const void *buf, size_t count);
    write_wrapper:
        mov rax, 1           
        mov rdi, rdi         
        mov rsi, rsi         
        mov rdx, rdx         
        syscall
        ret

.globl close_wrapper
    # Wrapper pentru close
    # int close(int fd);
    close_wrapper:
        mov rax, 3           
        mov rdi, rdi         
        syscall
        ret
