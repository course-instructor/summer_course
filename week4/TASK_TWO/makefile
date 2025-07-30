CC       = gcc
CFLAGS   = -Wall -Wextra -std=c11 -g
TARGET   = my_sniffer

SRCS     = src/my_sniffer.c
OBJS     = $(SRCS:.c=.o)
INCDIR  = headers
CFLAGS += -I$(INCDIR)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	sudo ./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
