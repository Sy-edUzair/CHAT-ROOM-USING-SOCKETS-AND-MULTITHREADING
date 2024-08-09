#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<pthread.h>
#include<sys/types.h>
#include<signal.h>

#define NUM_CLIENTS 10
#define BUFFER_SIZE 2048


static _Atomic unsigned int client_count = 0;//atomic variable to prevent access of more tha one thread
static int user_id = 0;

//Client Structure 
typedef struct {
	struct sockaddr_in address;
	int sockfd;//socket descriptor
	int uid; //id of client
	char name[32];
}client_struct;

client_struct* clients[NUM_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void overwrite(){
	printf("%s","> ");
	fflush(stdout);
}
void eliminate_enter(char arr[],int len){
	for(int i=0 ; i<len ; i++){
		if(arr[i] == '\n'){
			arr[i] = '\0';
			break;
		}
	}
}
void addclient(client_struct *client){
	pthread_mutex_lock(&clients_mutex);
	for(int i=0 ; i< NUM_CLIENTS ; i++){
		if(!clients[i]){
			clients[i]=client;
			break;
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}
void removeclient(int id){
	pthread_mutex_lock(&clients_mutex);
	for(int i=0 ; i< NUM_CLIENTS ; i++){
		if(clients[i]){
			if(clients[i]->uid == id){
				clients[i]=NULL;
				break;
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

void send_msg(char message[],int senderid){
	pthread_mutex_lock(&clients_mutex);
	
	for(int i=0;i<NUM_CLIENTS;i++){
		if(clients[i]){
			if(clients[i]->uid != senderid){//one client acts as a sender and sends messages to other clients except itself
				if(write(clients[i]->sockfd,message,strlen(message))<0){
					perror("WRITE ERROR: write to socket descriptor failed\n");
					break;
				}	
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);

}
void* handle_client(void* args){
	char buffer[BUFFER_SIZE];
	char name[32];
	int exit_flag=0;//This will tell us whteher to disconnect the client or exit loop when some error happens
	client_count++;
	client_struct* client = (client_struct*)args;
	
	//recive name from client
	if(recv(client->sockfd,name,32,0)<=0 || strlen(name)<2 || strlen(name)>=32-1){
		printf("Enter name correctly\n");
		exit_flag=1;
	}
	else{
		strcpy(client->name,name);//if name recieved then client has connected with the server
		sprintf(buffer,"%s has joined the chat\n",client->name);//send message to other clients that the current client has joined the chat
		printf("%s",buffer);
		send_msg(buffer,client->uid);	
	}
	
	bzero(buffer,BUFFER_SIZE);//Zero out value of buffer
	
	while(1){
		if(exit_flag == 1){
			break;
		}	
		int bytesreceived = recv(client->sockfd,buffer,BUFFER_SIZE,0);
		if(bytesreceived>0){
			if(strlen(buffer)>0){//this means that the buffer has recieved some message and we will send it out to other clients
				send_msg(buffer,client->uid);
				eliminate_enter(buffer,strlen(buffer));
				printf("%s\n",buffer);
			}	
		}	
		else if(bytesreceived == 0 || strcmp(buffer,"exit") == 0 ){
			sprintf(buffer,"%s has left\n",client->name);
			printf("%s",buffer);
			send_msg(buffer,client->uid);//send mesage to all other clients ke this person has left the chatroom
			exit_flag=1;
		}
		else{
			printf("ERROR:-1\n");
			exit_flag=1;
		}
		bzero(buffer,BUFFER_SIZE);
	}
	close(client->sockfd);//close client socket
	removeclient(client->uid);
	free(client);
	client_count--;
	
	pthread_detach(pthread_self());//graceful exit
	return NULL;
}
int main(){
	char *ip = "127.0.0.1";//local host
	
	int option = 1;
	int listenfd=0, connectfd=0;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	
	pthread_t tid;
	
	//Socket Settings
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	server_addr.sin_family= AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(ip);//network address
	server_addr.sin_port=htons(4444);
	
	//Signals
	signal(SIGPIPE, SIG_IGN);//Ignore pipe signals
	if(setsockopt(listenfd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option)) <0){
		perror("SETSOCKOPT ERROR\n");
		return EXIT_FAILURE;
	}
	
	//bind
	if(bind(listenfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
		perror("BIND ERROR!\n");
		return EXIT_FAILURE;
	}
	//listen
	if(listen(listenfd,10)<0){
		perror("LISTEN ERROR\n");
		return EXIT_FAILURE;
	}
	printf("--------WELCOME TO CHATROOM----------\n");
	//loop that will receive messages from clients and send it to other clients
	
	while(1){
		socklen_t client_len = sizeof(client_addr);
		connectfd = accept(listenfd,(struct sockaddr*)&client_addr,&client_len);
		//Check for max number of clients
		if ((client_count + 1)==NUM_CLIENTS){
			printf("CLIENT LIMIT REACHED!...CONNECTION REJECTED\n");
			printf(":%d\n",client_addr.sin_port);
			close(connectfd);
			continue;
		}
		
		client_struct* cli = (client_struct*)malloc(sizeof(client_struct));//creating client
		cli->address = client_addr;
		cli->sockfd = connectfd;
		cli->uid = user_id++;
		
		addclient(cli);
		pthread_create(&tid,NULL,&handle_client,(void*)cli);
		sleep(1);
	}
return EXIT_SUCCESS;
}
