CC := gcc
CFLAGS := -Wall

TARGET := main

all: $(TARGET)

$(TARGET): main.c wrapper.S
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean all

clean:
	rm -f $(TARGET)
