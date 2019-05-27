CC=gcc
CFLAGS=-g -I./include

all: packetize

obj:
	mkdir -p ./obj

lib:
	mkdir -p ./lib

./obj/next.o: ./src/next.c obj
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

./obj/conf.o: ./src/conf.c obj lib
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

./lib/libdaLib.so: ./obj/next.o ./obj/conf.o
	$(CC) -shared -o $@ $^

packetize: ./src/main.c ./lib/libdaLib.so
	$(CC) $(CFLAGS) -o $@ $< ./lib/libdaLib.so

LD_LIBRARY_PATH=$(pwd)/lib/libdaLib.so

clean:
	rm -f packetize
	rm -rf ./obj
	rm -rf ./lib
