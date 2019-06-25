CC          =	gcc
# Flags that are sent to the compiler  change the std if you wish

CFLAGS      =	-Wall -std=c11 -g -Iinclude
SRC = src/
BIN = bin/

all: list parser

list: ./src/LinkedListAPI.c
	$(CC) $(CFLAGS) -fpic -c ./src/LinkedListAPI.c -o ./bin/list.o
	$(CC) $(CFLAGS) -dynamiclib ./bin/list.o -o ./bin/liblist.dylib

parser: ./src/Helper.c ./src/CalendarParser.c
	$(CC) $(CFLAGS) -fpic -c ./src/Helper.c -o ./bin/helper.o
	$(CC) $(CFLAGS) -fpic -c ./src/CalendarParser.c -o ./bin/parser.o
	$(CC) $(CFLAGS) -fpic -c ./src/LinkedListAPI.c -o ./bin/list.o
	$(CC) $(CFLAGS) -dynamiclib ./bin/list.o ./bin/helper.o ./bin/parser.o -o ./bin/libcal.dylib

clean:
	rm -rf ./bin/*.o
	rm -rf ./bin/*.dylib
