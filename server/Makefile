TARGET = server

SRCS = server.c mon_cpu.c mon_net.c mon_report.c mon_time.c \
       mon_disk.c mon_ram.c mon_thread.c mon_udp.c
	
OBJS = $(SRCS:.c=.o)

CC = gcc

CFLAGS = -g -Wall -std=c99

LDFLAGS = -ljansson -lpthread

.PHONY: all clean

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
 
.c.o:
	$(CC) $(CFLAGS)  -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS)
