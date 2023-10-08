all: client server fancyclient

client: client.o
	cc -g -o client client.o

client.o: client.c
	cc -c -Wall -g client.c

fancyclient: fancyclient.o
	cc -g -o fancyclient fancyclient.o

fancyclient.o: fancyclient.c
	cc -c -Wall -g fancyclient.c

server: server.o
	cc -g -o server server.o

server.o: server.c
	cc -c -Wall -g server.c

clean:
	rm client.o client
	rm server.o server
	rm fancyclient.o fancyclient

