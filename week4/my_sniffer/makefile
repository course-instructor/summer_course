CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99


SRCS = SRC/my_sniffer.c main.c
OBJS = $(SRCS:.c=.o)
DEPS = HEADER/my_sniffer.h


TARGET = sniffer_program


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^


run: $(TARGET)
	./$(TARGET) 
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)