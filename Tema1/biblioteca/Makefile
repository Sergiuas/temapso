CC := gcc
CFLAGS := -Wall

all: main

main: main.c libso_stdio.so 
	$(CC) $(CFLAGS) $< -o $@ -L. -lso_stdio -Wl,-rpath=.

libso_stdio.so: so_stdio.o utils.o
	$(CC) $(CFLAGS) -shared $^ -o $@

so_stdio.o: so_stdio.c utils.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

clean: 
	rm -f main *.o *.so 

.PHONY: all clean
