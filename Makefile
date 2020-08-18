CC = g++
LD = g++

CFLAGS += -Wall -O2
CFLAGS += -l./inc
CFLAGS += -std=c++11

LDFLAGS += -std=c++11

OBJS += serverMain.cc clientMain.cc server.o client.o

all: $(OBJS)
	$(LD) $(LDFLAGS) serverMain.cc server.o -o chat_server
	$(LD) $(LDFLAGS) clientMain.cc client.o -o chat_client

server.o: server.cc server.h common.h
	$(CC) $(CFLAGS) -c server.cc

client.o: client.cc client.h common.h
	$(CC) $(CFLAGS) -c client.cc

.PHONY:clear
clear:
	rm -f *.o chat_server chat_client
	@echo "clear over!"                      