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
	//initilize and "clean" string
	char * byte_info = malloc(sizeof(char)*10);
	char * tmp_info = malloc(sizeof(char) *10);
	strcpy(byte_info,"0000000000");
	char * bufferstr = malloc(sizeof(char)*100);
	char * file = malloc(sizeof(char)*100);
	FILE * toread_file;
	toread_file = fopen("testfile.txt","r");
	fgets(bufferstr,100,toread_file);
	//bufferstr[strlen(bufferstr)]='\0';
	strcpy(file,"Mr.Roboot!");
	int sret;
	fd_set readfds;
	struct timeval timeout;

	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(PORT);

	int socket_client = socket(PF_INET, SOCK_STREAM, 0);
	connect(socket_client, (struct sockaddr *)&client, sizeof(struct sockaddr));
	int buffer_size = strlen(bufferstr);
	bufferstr[buffer_size]='\0';
	printf("%s\n",bufferstr);
	//get size of string and convert it into the 000... format. example for 2^10 byte 0000001028
	sprintf(tmp_info, "%i", buffer_size);
	strcpy(byte_info+(10-strlen(tmp_info)),tmp_info);
	printf("\n%s\n",byte_info);
	//sending to server
	send(socket_client, byte_info,10, 0);
	send(socket_client, bufferstr,buffer_size-1, 0);
	recv(socket_client, bufferstr,strlen(bufferstr),0);
	printf("%s\n",bufferstr);
	return EXIT_SUCCESS;
}
