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
char * getarg_string(int size, char input[size]){
	int pos;
	pos = strcspn(input," ")+1;
	char * arg = malloc(sizeof(char)*(strlen(input)-pos));
	if(pos<strlen(input)){
			strncpy(arg,input+pos,strlen(input)-pos);
			arg[strlen(input)]='\0';
			return arg;
		}
	return 0;
}

int recieving_data(int socket_server, FILE * newfile){
	int remaining;
	char bufferstr[2048];
	char bufferstr_size[10];
	recv(socket_server, bufferstr_size,10,0);
	printf("~%s\n",bufferstr_size);
	remaining = atoi(bufferstr_size);
	int set = remaining;
	//reading the information within the buffersize
	while((remaining > 0) && (recv(socket_server, bufferstr,remaining,0)>0)){
		printf("-%s",bufferstr);
		fwrite(bufferstr,1,remaining,newfile);
		remaining = remaining - strlen(bufferstr);
	}
	printf("File recieved!\n");
}

int gettingbytes_of_file(int quantity, char * bufferstr, FILE * send_file){
	strcpy(bufferstr,"");
	int difflength = quantity;
	while((difflength > 1) && (fgets(bufferstr+strlen(bufferstr),difflength,send_file)>0)){
		difflength = quantity - strlen(bufferstr);
	}
	return 1;
}

int sending_data(int size, char bufferstr[size],int socket_client){
	char byte_info[10];
	char tmp_info[10];
	//get size of string and convert it into the 000... format. example for 2^10 byte 0000001028
	strcpy(byte_info,"0000000000");
	int buffer_size = strlen(bufferstr);
	bufferstr[buffer_size]='\0';
	sprintf(tmp_info, "%i", buffer_size);
	strcpy(byte_info+(10-strlen(tmp_info)),tmp_info);
	send(socket_client, byte_info,10, 0);
	send(socket_client, bufferstr,strlen(bufferstr), 0);
	return 1;
}


int PORT = 1345;
int main(void) {
	struct sockaddr_in client;
	int communication_port;
	int size;
	size = 2048;
	//initilize and "clean" string
	char * bufferstr = malloc(sizeof(char)*2048);
	char * actionstr = malloc(sizeof(char)*100);
	char * action;
	char * arguments;

	char incoming[100];
	int sret;
	fd_set readfds;
	printf(">");
	fgets(actionstr,1024,stdin);
	//strcpy(actionstr,"add Nackmuhleareawsome");
	struct timeval timeout;
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	client.sin_port = htons(PORT);
	int socket_client = socket(PF_INET, SOCK_STREAM, 0);
	connect(socket_client, &client, sizeof(client));
	int buffer_size = strlen(bufferstr);
	bufferstr[buffer_size]='\0';

	//*******	connection to server	*******
	//sending size of bufferstr
	sending_data(strlen(actionstr),actionstr,socket_client);
	//recieve Port number
	recv(socket_client, incoming,5,0);
	communication_port = atoi(incoming);
	action = getfirst_string(strlen(actionstr),actionstr);
	arguments = getarg_string(strlen(actionstr),actionstr);
	//*** sending file to server ***
	if(strcmp("add",action)==0){

		FILE * toread_file;
		toread_file = fopen("testfile.txt","r+");
		if(toread_file==0){
			perror("Error: ");
			return -1;
		}
		fseek(toread_file,0,SEEK_SET);
		gettingbytes_of_file(size,bufferstr,toread_file);


		client.sin_port = htons(communication_port);
		socket_client = socket(PF_INET, SOCK_STREAM, 0);
		connect(socket_client, &client, sizeof(client));
		sending_data(strlen(bufferstr),bufferstr,socket_client);
	}else if(strcmp("get",action)==0){
		FILE * getfile;
		getfile = fopen(arguments,"w");
		if(getfile==0){
			perror("Error: ");
			return -1;
		}
		client.sin_port = htons(communication_port);
		socket_client = socket(PF_INET, SOCK_STREAM, 0);
		connect(socket_client, &client, sizeof(client));
		recieving_data(socket_client, getfile);
	}else{
		printf("Invalid input: %s\n",actionstr);
	}
	printf("Success");
	return EXIT_SUCCESS;
}
