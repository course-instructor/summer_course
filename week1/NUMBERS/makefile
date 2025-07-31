CC = gcc

CFLAGS = -Wall -Wextra -g 

SRCS = main.c utils.c

OBJS = $(SRCS:.c=.o)

TARGET = NUMBERS

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)