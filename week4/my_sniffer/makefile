CC = gcc
CFLAGS = -I./include -Wall -Wextra -g -ansi -std=c99


SRCS = my_sniffer.c src/sniffer.c src/input.c
OBJS = $(SRCS:.c=.o)


TARGET = sniffer_program


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^


run: $(TARGET)
	sudo ./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) ./temporary_files/offset_file.bin ./temporary_files/tmp_file.bin
