
//chatclient.c

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<sys/time.h>
#include<sys/types.h>

#define PORT 1573
#define BUFSIZE 2048

int main(int argc, char *argv[])
{
	int sockfd;
	fd_set sockset; //套接字集合，用于判断是套接字还是I/O输入
	struct sockaddr_in server;
	struct sockaddr_in client;
	int recvnum;
	char sendbuf[BUFSIZE];
	char recvbuf[BUFSIZE];
	int length;

	if(2>argc)
	{
		printf("please input ip!\n");
		exit(1);
	}

	if(-1==(sockfd = socket(AF_INET,SOCK_STREAM,0)))
	{
		perror("create client socket error!\n");
		exit(1);
	}

	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(PORT);

	if(-1==connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))
	{
		perror("client connect error!\n");
		exit(1);
	}

	memset(sendbuf,0,2048);
	fprintf(stderr,"welcome to visit the chat server\n");
	fprintf(stderr,"please input your name:");
	fgets(sendbuf,256,stdin);

	if(0>send(sockfd,sendbuf,strlen(sendbuf),0))
	{
		perror("sending data error!\n");
		close(sockfd);
		exit(1);
	}

	//初始化集合
	FD_ZERO(&sockset);
	FD_SET(sockfd,&sockset);
	FD_SET(0,&sockset);

	while(1)
	{
		memset(recvbuf,0,sizeof(recvbuf));
		memset(sendbuf,0,sizeof(sendbuf));
		select(sockfd+1,&sockset,NULL,NULL,NULL);
		if(FD_ISSET(sockfd,&sockset))
		{
			//处理：如果是套接字被激活，表示服务器有信息传过来，进行接收处理
			recvnum=read(sockfd,recvbuf,sizeof(recvbuf));
			recvbuf[recvnum]='\0';
			printf("%s\n",recvbuf);
			printf("\n");
			fflush(stdout);
		}
		if(FD_ISSET(0,&sockset))
		{
			//处理：如果是I/O被激活，表示客户有消息要发送出去，进行发送处理
			fgets(sendbuf,sizeof(sendbuf),stdin);
			length = strlen(sendbuf);
			sendbuf[length] = '\0';

       		if(strcmp(sendbuf,"/help\n")==0)
       		{
				//处理，输入"/help"表示想要得到帮助信息，帮助信息是存储在客户端的，不用向服务器发送信息
				//跳过继续执行循环
				printf("\n");
               	fprintf(stderr,"/help show the help message\n");
				fprintf(stderr,"/send usage:/send user message send message to user\n");
               	fprintf(stderr,"/who show who is online\n");
               	fprintf(stderr,"/quit quit from server\n");
				printf("\n");
				continue;
       		}
			write(sockfd,sendbuf,sizeof(sendbuf));
			if(strcmp(sendbuf,"/quit\n")==0)
			{
				//处理，客户想要退出，关闭套接字，用户程序退出
				printf("quiting from chat room!\n");
				close(sockfd);
				exit(1);
			}
		}
		FD_ZERO(&sockset);
		FD_SET(sockfd,&sockset);
		FD_SET(0,&sockset);
	}
	close(sockfd);
	return 0;
}
