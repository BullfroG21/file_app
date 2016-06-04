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
#include <string.h>

#include <pthread.h>

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>

struct parameter{
	struct sockaddr_in server;
	int main_socket;
	fd_set readfds;
};

void * server_thread(void * infos){
	FILE * newfile;
	newfile = fopen("newfile","w");
	struct parameter * parainfos = infos;
	char bufferstr[2048];
	char * bufferstr_size = malloc(sizeof(char)*10);
	int testfd = socket(PF_INET, SOCK_STREAM, 0);
	int len = sizeof(parainfos->server);
	FD_SET(testfd,&parainfos->readfds);
	if((testfd = accept(parainfos->main_socket, (struct sockaddr *)&parainfos->server, &len)) < 0){
		printf("Fehler bei accept");
		return -1;
	}
	printf("incomming: %i vs. set %i \n",testfd,parainfos->main_socket);
	//getting the size of the information
	int remaining;
	recv(testfd, bufferstr_size,10,0);
	remaining = atoi(bufferstr_size);
	printf("\n%i\n",remaining);
	int set = remaining;
	//reading the information within the buffersize
	while((remaining > 0) && (recv(testfd, bufferstr,remaining,0)>0)){
		//memset(bufferstr,0,strlen(bufferstr));
		fwrite(bufferstr,1,remaining,newfile);
		remaining = remaining - remaining;
	}
	fclose(newfile);
	//setting the end of the string and print it (just for testing purposes)
	bufferstr[set-1]='\0';
	printf("%s\n",bufferstr);
	//read(testfd,bufferstr, 100);
	//printf("%s\n",bufferstr);
	send(testfd, "arknowledge!", strlen("arknowledge!"), 0);
	memset(bufferstr,0,strlen(bufferstr));
	close(testfd);
	return 0;
}

void * testingint(int infos){
	printf("halo");
	return 0;
}

int PORT = 1345;
int main(void) {
	struct parameter parainfos;
	int i;
	pthread_t threadIT;
	struct sockaddr_in server, client;
	int sret;
	char bufferstr[100];
	fd_set readfds;
	struct timeval timeout;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);
	int sock1 = socket(PF_INET, SOCK_STREAM, 0);
	bind(sock1, (struct sockaddr*) &server, sizeof(server));
	if(listen(sock1, 5) == -1 )
	         return -1;
	while(1){
		printf("Waiting for information!\n");
		FD_ZERO(&readfds);
		FD_SET(sock1,&readfds);
		timeout.tv_sec=5;
		timeout.tv_usec = 0;
		sret = select(sock1+1,&readfds,NULL,NULL,&timeout);
		//check if regular input or timeout
		if(sret == 0){
			printf("Timeout!\n");
		}else{
			parainfos.main_socket=sock1; parainfos.readfds=readfds; parainfos.server=server;
			int i = 0; int result;
			pthread_create(&threadIT,NULL,server_thread,&parainfos);
			//printf("Socket connection initialisert!\n");
			printf("waiting for thread ...\n");
		//	server_thread(server,sock1,readfds);
			pthread_join(threadIT,NULL);
		}
	}
	printf("Server wurde beendet!\n");
	return EXIT_SUCCESS;
}
