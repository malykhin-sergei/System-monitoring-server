TARGET = client

SRCS = client.c rep_stat.c
	
OBJS = $(SRCS:.c=.o)

CC = gcc

CFLAGS = -g -Wall -std=c99

LDFLAGS = -ljansson

.PHONY: all clean

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
 
.c.o:
	$(CC) $(CFLAGS)  -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS)
