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
#include <time.h>

#include <errno.h>

struct parameter{
	struct sockaddr_in server;
	int main_socket;
	fd_set readfds;
};

int gettingbytes_of_file(int quantity, char * bufferstr, FILE * send_file){
	strcpy(bufferstr,"");
	int difflength = quantity;
	while((difflength > 1) && (fgets(bufferstr+strlen(bufferstr),difflength,send_file)>0)){
		difflength = quantity - strlen(bufferstr);
	}
	return 1;
}

int get_randomport(void){
	int random_port;
	srand(time(NULL));
	random_port = rand()%60000+1235;
	return random_port;
}

int recieving_data(int socket_server, FILE * newfile){
	int remaining;
	char bufferstr[2048];
	char bufferstr_size[10];
	recv(socket_server, bufferstr_size,10,0);
	remaining = atoi(bufferstr_size);
	int set = remaining;
	//reading the information within the buffersize
	while((remaining > 0) && (recv(socket_server, bufferstr,remaining,0)>0)){
		//memset(bufferstr,0,strlen(bufferstr));
		fwrite(bufferstr,1,remaining,newfile);
		remaining = remaining - strlen(bufferstr);
	}
	printf("File recieved!\n");
}

char * getfirst_string(int size, char input[size]){
	int pos;
	pos = strcspn(input," ");
	char * action = malloc(sizeof(char)*pos);
	//"Switch"
	if(pos<strlen(input)){
		strncpy(action,input,pos);
		action[pos]='\0';
		return action;
	}
	return 0;
}
int sending_data(int size, char bufferstr[size],int socket_client){
	char byte_info[10];
	char tmp_info[10];
	//get size of string and convert it into the 000... format. example for 2^10 byte 0000001028
	strcpy(byte_info,"0000000000");
	int buffer_size = strlen(bufferstr);
	sprintf(tmp_info, "%i", buffer_size);
	strcpy(byte_info+(10-strlen(tmp_info)-1),tmp_info);
	send(socket_client, byte_info,10, 0);
	send(socket_client, bufferstr,strlen(bufferstr), 0);
	return 1;
}
char * getarg_string(int size, char input[size]){
	int pos;
	pos = strcspn(input," ")+1;
	char * arg = malloc(sizeof(char)*(strlen(input)-pos));
	if(pos<strlen(input)){
			strncpy(arg,input+pos,strlen(input)-pos);
			arg[strlen(arg)-1]='\0';
			return arg;
		}
	return 0;
}
int run_action(int port, int size, char input[size]){
	char * action;
	char * arguments;
	arguments = getarg_string(strlen(input),input);
	action = getfirst_string(strlen(input),input);
	if(strcmp("add",action)==0){
		add_files(port,strlen(arguments),arguments);
	}else if(strcmp("get",action)==0){
		get_files(port,strlen(arguments),arguments);
		//int get_files(int PORT,int size,char actionstr[size])
	}else{
		printf("Invalid action: %s\n",action);
	}
}

void * start_communication(void * infos){
	struct parameter * parainfos = infos;
	char bufferstr[2048];
	char * action;
	char * ark_port = malloc(sizeof(char)*6);
	char * bufferstr_size = malloc(sizeof(char)*10);
	int random_port;
	int testfd = socket(PF_INET, SOCK_STREAM, 0);
	int len = sizeof(parainfos->server);
	FD_SET(testfd,&parainfos->readfds);
	if((testfd = accept(parainfos->main_socket, (struct sockaddr *)&parainfos->server, &len)) < 0){
		printf("Fehler bei accept");
		return -1;
	}
	//getting the size of the information
	int remaining;
	recv(testfd, bufferstr_size,10,0);
	remaining = atoi(bufferstr_size);
	int set = remaining;
	//reading the information within the buffersize
	while((remaining > 0) && (recv(testfd, bufferstr+(set-remaining),remaining,0)>0)){
		//memset(bufferstr,0,strlen(bufferstr));
		remaining = remaining - strlen(bufferstr);
	}
	//setting the end of the string and print it (just for testing purposes)
	bufferstr[set]='\0';

	char * arguments;
	arguments = getarg_string(strlen(bufferstr),bufferstr);
	action = getfirst_string(strlen(bufferstr),bufferstr);
	random_port = get_randomport();
	sprintf(ark_port, "%i", random_port);
	send(testfd, ark_port, strlen(ark_port), 0);
	close(testfd);
	close(parainfos->main_socket);
	run_action(random_port, strlen(bufferstr), &bufferstr);
	memset(bufferstr,0,strlen(bufferstr));
//	sending_files(random_port,strlen(bufferstr),bufferstr);
	return 0;
}


int get_files(int port,int size,char actionstr[size]){
	char * bufferstr = malloc(sizeof(char)*2048);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	int file_socket = socket(PF_INET, SOCK_STREAM, 0);
	bind(file_socket, (struct sockaddr*) &server, sizeof(server));
	if(listen(file_socket, 5) == -1 ){
		printf("Failed by listening");
        return -4;
	}
	FILE * requiredfile;
	requiredfile = fopen(actionstr,"r");
	int testfd = socket(PF_INET, SOCK_STREAM, 0);
	int len = sizeof(server);
	if((testfd = accept(file_socket, (struct sockaddr *)&server, &len)) < 0){
		printf("Fehler bei accept");
		return -1;
	}
	fseek(requiredfile,0,SEEK_SET);
	actionstr[strlen(actionstr)]='\0';
	gettingbytes_of_file(2048,bufferstr,requiredfile);
	sending_data(strlen(bufferstr), bufferstr,testfd);
	close(testfd);
	return 0;
}

int add_files(int PORT,int size,char bufferstr[size]){
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);
	int file_socket = socket(PF_INET, SOCK_STREAM, 0);
	bind(file_socket, (struct sockaddr*) &server, sizeof(server));
	if(listen(file_socket, 5) == -1 ){
		printf("Failed by listening");
        return -4;
	}
	FILE * newfile;
	newfile = fopen(bufferstr,"w");
	int testfd = socket(PF_INET, SOCK_STREAM, 0);
	int len = sizeof(server);
	if((testfd = accept(file_socket, (struct sockaddr *)&server, &len)) < 0){
		printf("Fehler bei accept");
		return -1;
	}
	recieving_data(testfd,newfile);
	close(testfd);
	return 0;
}


int MAIN_PORT = 1345;
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
	server.sin_port = htons(MAIN_PORT);
	int sock1 = socket(PF_INET, SOCK_STREAM, 0);
	bind(sock1, (struct sockaddr*) &server, sizeof(server));
	if(listen(sock1, 5) == -1 )
	         return -1;
	//while(1){
		printf("Waiting for information!\n");
		FD_ZERO(&readfds);
		FD_SET(sock1,&readfds);
		timeout.tv_sec=15;
		timeout.tv_usec = 0;
		sret = select(sock1+1,&readfds,NULL,NULL,&timeout);
		//check if regular input or timeout
		if(sret == 0){
			printf("Timeout!\n");
		}else{
			parainfos.main_socket=sock1; parainfos.readfds=readfds; parainfos.server=server;
			int i = 0; int result;
			pthread_create(&threadIT,NULL,start_communication,&parainfos);
			printf("waiting for thread ...\n");
		//	server_thread(server,sock1,readfds);
			pthread_join(threadIT,NULL);
		}
	//}
	printf("Server wurde beendet!\n");
	return EXIT_SUCCESS;
}
