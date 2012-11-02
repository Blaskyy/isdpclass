/*
 * =====================================================================================
 *
 *       Filename:  tcpclient.c
 *
 *    Description:  This progarm is demostrate to how to write a tcp remote control client
 *
 *        Version:  1.0
 *        Created:  2010年09月11日 22时32分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Gang Liang (cs.scu.edu.cn/~lianggang), lianggang@scu.edu.cn
 *        Company:  Sichuan university
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8900
#define BUFSIZE 2048


void printusage(char*command)
{
	if (NULL==command)
		exit(-1);
	
	fprintf(stderr,"the useage of %s :",command);
	fprintf(stderr,"%s IPADDR\n",command);
	
	return;

}


int main(int argc,char** argv)
{
	int sockfd;
	int length;
	struct sockaddr_in server;
	int sndnum;
	int recvnum;
	char sendbuf[BUFSIZE];
	char recvbuf[BUFSIZE];
	
	sockfd=-1;
	length=0;

	if (2!=argc)
	{
		printusage(argv[0]);
		return -1;
	}


	if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
	{
		perror("create socket error\n");
		return -1;
	}


	memset(&server,0,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(PORT);

	if (-1==connect(sockfd,(struct sockaddr*)&server,sizeof(server)))
	{
		perror("connect error\n");
		close(sockfd);
		return -1;

	}


	while(1)

	{
		fprintf(stderr,"TCP>");
		
		memset(recvbuf,0,BUFSIZE);
		memset(sendbuf,0,BUFSIZE);
		fgets(sendbuf,BUFSIZE,stdin);
		length=strlen(sendbuf);
		sendbuf[length-1]='\0';

		if (0>=(sndnum=write(sockfd,sendbuf,strlen(sendbuf))))
		{
			perror("send error\n");
			close(sockfd);
			exit(-1);
		}

		if (0==strcmp(sendbuf,"quit"))
		{
			fprintf(stderr,"quit...\n");
			close(sockfd);
			exit(0);
		}

		if (0>=(recvnum=read(sockfd,recvbuf,BUFSIZE)))
		{
			perror("read error\n");
			close(sockfd);
			exit(-1);
		}
		
		recvbuf[recvnum]='\0';
		fprintf(stderr,"the result:\n");
		fprintf(stderr,"%s\n",recvbuf);
			
	}

}
