all: mutex  conclient concserver

clean: 
	rm -f mutex mutex.o
	rm -f concserver concserver.o
	rm -f conclient conclient.o

mutex: mutex.o
	cc -g -o mutex mutex.o -pthread

concserver: concserver.o
	cc -g -o concserver concserver.o -pthread

conclient: conclient.o
	cc -g -o conclient conclient.o -pthread



%.o: %.c
	cc -g3 -Wall -o $@ -c $<
