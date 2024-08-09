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

#define SIZE 2048
volatile sig_atomic_t flag=0;
int sockfd=0;
char name[32];

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

void exitProgramthroughCtrlC(int signum){
	flag=1;
}

void send_msg_handler(){
	char buffer[SIZE]={};
	char msg[SIZE +32]={};//message will have message and name so its length is size + 50
	while(1){
		overwrite();
		fgets(buffer,SIZE,stdin);//get message from other client through server
		eliminate_enter(buffer,SIZE);
		
		if(strcmp(buffer,"exit") == 0){
			break;
		}
		else{
			sprintf(msg,"%s: %s\n",name,buffer); 
			send(sockfd,msg,strlen(msg),0);
		}
	bzero(buffer,SIZE);
	bzero(msg,(SIZE+32));
	}
exitProgramthroughCtrlC(2);
}
void receive_msg_handler(){
	char message[SIZE]={};
	while(1){
		int receive = recv(sockfd,message,SIZE,0);
		if(receive > 0){//it received some number of bytes
			printf("%s",message);
			overwrite();
		}
		else if(receive == 0){
			break;
		}
		memset(message,0,sizeof(message));
	}
}

int main(){
	char *ip = "127.0.0.1";//local host
	signal(SIGINT,exitProgramthroughCtrlC);
	
	printf("Enter your name: ");
	fgets(name,32,stdin);
	eliminate_enter(name,strlen(name));
	
	if(strlen(name)>31 || strlen(name)<2){
		printf("Enter name correctly...\n");
		return EXIT_FAILURE;
	}
	struct sockaddr_in server_addr;
	//Socket values
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	server_addr.sin_family= AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(ip);
	server_addr.sin_port=htons(4444);
	
	//Connect client to server
	int request = connect(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(request==-1){
		printf("CONNECTION ERROR\n");
		return EXIT_FAILURE;
	}
	//send name
	send(sockfd,name,32,0);
	printf("-----WELCOME TO CHATROOM-----\n");
	
	pthread_t sendingthread;
	pthread_t receivingthread;
	
	if(pthread_create(&sendingthread,NULL,(void*)send_msg_handler,NULL)!=0){
		printf("Error: pthread\n");
		return EXIT_FAILURE;
	}
	
	if(pthread_create(&receivingthread,NULL,(void*)receive_msg_handler,NULL)!=0){
		printf("Error: pthread\n");
		return EXIT_FAILURE;
	}
	
	while(1){
		if(flag){
			printf("\nBYE\n");
			break;
		}
	}
close(sockfd);	
return EXIT_SUCCESS;
}
