/*
 ============================================================================
 Name        : file_client.c
 Author      : Gwechenberger Florian
 Version     :
 Copyright   : Your copyright notice
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


int PORT = 1344;
int main(void) {
	struct sockaddr_in client;
	char * file = malloc(sizeof(char)*100);
	strcpy(file,"Mr.Roboot!");
	int sret;
	char bufferstr[100];
	fd_set readfds;
	struct timeval timeout;

	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(PORT);
	int socket_client = socket(PF_INET, SOCK_STREAM, 0);
	connect(socket_client, (struct sockaddr *)&client, sizeof(struct sockaddr));
	send(socket_client, file, strlen(file), 0);
	recv(socket_client, bufferstr,100,0);
	printf("%s\n",bufferstr);
	return EXIT_SUCCESS;
}
