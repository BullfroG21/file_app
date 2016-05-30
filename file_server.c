/*
 ============================================================================
 Name        : file_server.c
 Author      : Gwechenberger Florian
 Version     :
 Copyright  dcd : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>


#include <errno.h>

#include <string.h>

int main(void) {
	struct sockaddr_in server;
	int sret;
	char bufferstr[100];
	fd_set readfds;
	struct timeval timeout;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(1344);
	int sock1 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(sock1, (struct sockaddr*) &server, sizeof(server));
	if(listen(sock1, 5) == -1 )
	         return -1;
	while(1){
		printf("Waiting for information!\n");
		FD_ZERO(&readfds);
		FD_SET(sock1,&readfds);
		timeout.tv_sec=5;
		timeout.tv_usec = 0;
		sret = select(10,&readfds,NULL,NULL,&timeout);
		if(sret == 0){
			printf("Timeout!\n");
		}
		else{
			read(sock1,bufferstr, 100);
			printf("%s",bufferstr);
			close(sock1);
			sock1 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			bind(sock1, (struct sockaddr*) &server, sizeof(server));
			if(listen(sock1, 5) == -1 )
				         return -1;
		}
	}
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
