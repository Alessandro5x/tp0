#include "common.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

struct client_data {
    int csock;
    struct sockaddr_storage storage;
};

// Criação das senhas aleatórias
char *randompass(int x){
    srand((unsigned int)(time(NULL)));
    int i;
    static char pass[9]; 

    for (i = 0; i < 4; i++) 
    {
        pass[ (2 * i)] = '0' + (rand() % 10); //generating numeric character
        char capLetter = 'A' + (rand() % 26 - x); //generating upper case alpha character
        pass[(2 * i) + 1] = capLetter;
    }
    pass[3 * i] = '\0'; //placing null terminating character at the end
    return pass;
}

char *passaluno;
char *passprofessor;

void * client_thread(void *data) {
    struct client_data *cdata = (struct client_data *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&cdata->storage);

    char caddrstr[BUFSZ];
    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[log] connection from %s\n", caddrstr);

// Recebimento da senha 
    char vsenha[9];
    size_t count = recv(cdata->csock, &vsenha, 9, 0);
    printf("aluno %s\n",passaluno);
    printf("professor %s\n",passprofessor);
    printf("[senha] %s, %d bytes: %s\n", vsenha, BUFSZ+1, caddrstr);

//Envio de Ok e da solicitação de matricula
    int matriculas[40];
    int posmatricula = 0;

void preenchematricula(int z){
    posmatricula = posmatricula +1;

    matriculas[posmatricula] = z;
}

//Senha de aluno
    if (strcmp(vsenha,passaluno) == 0){ 
        send(cdata->csock, "OK", sizeof("OK") + 1, 0);

        send(cdata->csock, "MATRICULA", sizeof("MATRICULA") + 1, 0);

        recv(cdata->csock, &matriculas[1], sizeof(int)+1, 0);

        printf("matricula>%d\n", ntohl(matriculas[1]));
        preenchematricula(ntohl(matriculas[1]));

        send(cdata->csock, "OK", sizeof("OK") + 1, 0);

    }else if(strcmp(vsenha,passprofessor)==0){
        send(cdata->csock, "OK", sizeof("OK") + 1, 0);
        printf("senha do professor\n");
        printf("As matriculas ate agora sao \n %d\n", ntohl(matriculas[1]));
       // for (int i = 0; i < sizeof(matriculas);i++){
         //   send(cdata->csock, &matriculas[i], sizeof(matriculas) + 1, 0);
      //  }
        printf("Pos matricula %d\n",posmatricula);
        send(cdata->csock, "\0", sizeof("OK") + 1, 0);

        char cok[BUFSZ];
        memset(cok, 0, BUFSZ);
        size_t countok = recv(cdata->csock, cok, strlen(cok) + 1, 0);
        if (countok != strlen(cok) + 1) {
           logexit("send");
        }

    }else{
        logexit("Invalid password");
        send(cdata->csock, "Invalid password", sizeof("Invalid password") + 1, 0);
        close(cdata->csock);
        free(cdata);
    }

    
    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
    count = send(cdata->csock, buf, strlen(buf) + 1, 0);
    if (count != strlen(buf) + 1) {
        logexit("send");
    }
    close(cdata->csock);
    free(cdata);

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {

    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

	passaluno = randompass(0);
    printf("Senha do professor: ");
	printf("%s\n", passaluno);
	passprofessor = randompass(1);
    printf("Senha dos alunos é: ");
	printf("%s\n", passprofessor);

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

	struct client_data *cdata = malloc(sizeof(*cdata));
	if (!cdata) {
		logexit("malloc");
	}
	cdata->csock = csock;
	memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
        char caddrstr2[BUFSZ];
    addrtostr(caddr, caddrstr2, BUFSZ);
    //READY
	size_t cnt = send(cdata->csock,"READY", sizeof("READY"), 0);
	if (cnt != sizeof("READY")) {
		logexit("send");
    }
    }
    exit(EXIT_SUCCESS);
}
