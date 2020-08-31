all:
	gcc -Wall -c common.c
	gcc -Wall client-aluno.c common.o -o client-aluno
	gcc -Wall client-prof.c common.o -o client-prof
	gcc -Wall server.c common.o -o server
	gcc -Wall -lpthread servidor.c common.o -o servidor -lpthread

clean:
	rm common.o client-aluno server servidor
	rm common.o client-prof server servidor
