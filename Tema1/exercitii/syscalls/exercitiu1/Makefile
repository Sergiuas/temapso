CC := gcc
CFLAGS := -Wall

TARGET := main

all: $(TARGET)

$(TARGET): main.c wrapper.o
	$(CC) $(CFLAGS) $^ -o $@

wrapper.o: wrapper.s
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean all

clean:
	rm -f $(TARGET) wrapper.o
