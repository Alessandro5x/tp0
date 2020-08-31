#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port> <senha>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

#define BUFSZ 1024

int main(int argc, char **argv) {
	if (argc < 4) {
		usage(argc, argv);}

	if (argc == 4) {
		char* senha = argv[3];

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

	printf("connected to %s\n", addrstr);

		char buf[BUFSZ];
		size_t i = send(s, senha, 9, 0);
		if (i != strlen(buf)+1) {
			logexit("send");
		}

	memset(buf, 0, BUFSZ);
	recv(s, buf, BUFSZ, 0);
	puts(buf);

	if (strcmp(buf,"READY") != 0){ 
		close(s);
	}

	recv(s, buf, BUFSZ, 0);
		puts(buf);	
	memset(buf,0,BUFSZ);
	recv(s, buf, BUFSZ, 0);
		puts(buf);
	if (strcmp(buf,"MATRICULA") == 0){ 
		int vmatricula;
		printf("Digite sua matricula e de enter>");
		scanf("%d", &vmatricula);
		int converted_matricula = htonl(vmatricula);
		size_t countmat = send(s, &converted_matricula, sizeof(vmatricula)+1, 0);
		if (countmat != sizeof(vmatricula)+1) {
			logexit("sendmatricula");
		}
		recv(s, buf, BUFSZ, 0);
			puts(buf);
			memset(buf,0,BUFSZ);
	}

	memset(buf, 0, BUFSZ);
	unsigned total = 0;
	while(1) {
		size_t count = recv(s, buf + total, BUFSZ - total, 0);
		if (count == 0) {
			// Connection terminated.
			break;
		}
		total += count;
	}
	close(s);

	printf("received %u bytes\n", total);
	puts(buf);

	exit(EXIT_SUCCESS);
	}
}