CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = todo

.PHONY: all clean

all: $(TARGET)

$(TARGET): todo.c
	$(CC) $(CFLAGS) -o $(TARGET) todo.c

clean:
	rm -f $(TARGET) $(TARGET).exe todo.txt todo.txt.tmp
