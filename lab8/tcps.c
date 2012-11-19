#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<dirent.h>
#include<pthread.h>

#define PORT 8901
int recvnum;
char recv_buf[2048];
char send_buf[2048];
char cmd[2048];

int execute(char* command,char* buf);
void* func(void *connect);

int main(int argc,char** argv){
	struct sockaddr_in server;
	struct sockaddr_in client;
	int len;
	int port;
	int listend;
	int connectd;
	int sendnum;
	int opt;

	pthread_t pt;
	int ret;

	pid_t t;

	port= PORT;
	memset(send_buf,0,2048);
	memset(recv_buf,0,2048);
	opt = SO_REUSEADDR;

	if (-1==(listend=socket(AF_INET,SOCK_STREAM,0))){
		perror("create listen socket error\n");
		exit(1);
	}
	setsockopt(listend,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	if (-1==bind(listend,(struct sockaddr *)&server,sizeof(struct sockaddr))){
		perror("bind error\n");
		exit(1);
	}

	if (-1==listen(listend,5)){
		perror("listen error\n");
		exit(1);
    }
	len=sizeof(struct sockaddr_in);
	while (1){
		memset(recv_buf,0,2048);
		memset(send_buf,0,2048);
		int *connectd;
		connectd=(int*)malloc(sizeof(int));
        if (-1==(*connectd=accept(listend,(struct sockaddr*)&client,&len))){
			perror("create connect socket error\n");
			continue;
    	}
		ret=pthread_create(&pt,NULL,(void*)func,*connectd);
		if(ret!=0){
			printf("线程建立失败");
			return 1;
		}
		close(connectd);
		continue;
	}
	close(listend);
	exit(1);
}

void* func(void *connect){
	int *connectd=connect;
	while(1){
		if(0>(recvnum = recv(connectd,recv_buf,sizeof(recv_buf),0))){
			perror("recv error\n");
			close(connectd);
			break;
		}
		recv_buf[recvnum]='\0';
		if (0==strcmp(recv_buf,"quit")){
			perror("quitting remote controling\n");
			close(connectd);
			continue;
		}
		printf("the message is: %s\n",recv_buf);

		strcpy(cmd,"sh -c ");
		strcat(cmd,recv_buf);
		execute(cmd,send_buf);

		if ('\0'==*send_buf)
			sprintf(send_buf,"command is not vaild ,check it please\n");

		printf("the server message is:%s\n",send_buf);

		if (0>send(connectd,send_buf,sizeof(send_buf),0)){
			perror("sending data error\n");
			continue;
		}

	}
}

int execute(char* command,char* buf){
	FILE * 	fp;
	int count;

	if (NULL==(fp = popen(command,"r"))){
		perror("creating pipe error\n");
		exit(1);
	}

	count = 0 ;

	while(((buf[count] = fgetc(fp))!=EOF)&&count<2047)
		count++;
	buf[count]='\0';

	pclose(fp);
	return count;
}






