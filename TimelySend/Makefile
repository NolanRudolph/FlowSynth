CC=gcc
CFLAGS=-g -O3 -I./include

all: packetize

obj:
	mkdir -p ./obj

lib:
	mkdir -p ./lib

./obj/next.o: ./src/next.c obj
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

./obj/conf.o: ./src/conf.c obj
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

./obj/scheduler.o: ./src/scheduler.c obj lib
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

./lib/daLib.so: ./obj/next.o ./obj/conf.o ./obj/scheduler.o
	$(CC) -shared -o $@ $^

packetize: ./src/main.c ./lib/daLib.so
	$(CC) $(CFLAGS) -o $@ $< ./lib/daLib.so

LD_LIBRARY_PATH=$(pwd)/lib/daLib.so

clean:
	rm -f packetize
	rm -rf ./obj
	rm -rf ./lib
