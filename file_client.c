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

int gettingbytes_of_file(int quantity, char * bufferstr, FILE * send_file){
	strcpy(bufferstr,"");
	int difflength = quantity;
	while((difflength > 1) && (fgets(bufferstr+strlen(bufferstr),difflength,send_file)>0)){
		difflength = quantity - strlen(bufferstr);
		//printf("Wort: %s\n",bufferstr);
		//printf("length: %i\n",strlen(bufferstr));
		//printf("\n#%i\n",difflength);
	}
	return 1;
}


int PORT = 1345;
int main(void) {
	struct sockaddr_in client;

	int size;
	size = 2048;
	//initilize and "clean" string
	char * byte_info = malloc(sizeof(char)*10);
	char * tmp_info = malloc(sizeof(char) *10);
	strcpy(byte_info,"0000000000");
	char * bufferstr = malloc(sizeof(char)*2048);
	char incoming[100];
	FILE * toread_file;
	toread_file = fopen("testfile.txt","r+");
	if(toread_file==0){
		perror("Error: ");
		return -1;
	}
	fseek(toread_file,0,SEEK_SET);
	gettingbytes_of_file(size,bufferstr,toread_file);
	int sret;
	fd_set readfds;
	struct timeval timeout;
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(PORT);
	int socket_client = socket(PF_INET, SOCK_STREAM, 0);
	connect(socket_client, &client, sizeof(client));
	int buffer_size = strlen(bufferstr);
	bufferstr[buffer_size]='\0';
	//get size of string and convert it into the 000... format. example for 2^10 byte 0000001028
	sprintf(tmp_info, "%i", buffer_size);
	strcpy(byte_info+(10-strlen(tmp_info)),tmp_info);
	//sending to server
	send(socket_client, byte_info,10, 0);
	send(socket_client, bufferstr,buffer_size, 0);
	recv(socket_client, incoming,strlen(bufferstr),0);
	printf("%s\n",incoming);
	close(socket_client);
	return EXIT_SUCCESS;
}
