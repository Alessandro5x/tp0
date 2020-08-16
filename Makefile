all:
	gcc -Wall -c common.c
	gcc -Wall client.c common.o -o client
	gcc -Wall server.c common.o -o server
	gcc -Wall -lpthread server-mt.c common.o -o server-mt -lpthread

clean:
	rm common.o client server server-mt
