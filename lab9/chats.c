/*C写的

要求：
用户默认处于广播模式，一个客户在其客户端发送的消息，其它客户端用户全部可以收到；
程序支持下列命令
	/help:显示帮助信息（思考：信息是放在客户端还是服务器端）；
/quit:用户退出聊天室，同时将退出信息广播给其他用户；
 /who:显示在线用户；

 /send 用户名 消息：向指定用户发送点到点消息
*/
//chatserver.c

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<sys/time.h>
#include<sys/types.h>

#define PORT 1573
#define BACKLOG 10
#define BUFSIZE 2048

//定义一个结构体，使得客户的信息可以结合到一起
struct client_info{
	int id;  //表示用户现在接入的套接字
	char name[256];
	int first;  //表示用户是不是第一次访问，用于传入名字
};

int main(){
	fd_set allset;   //需要扫描的所有套接字
	fd_set rset;  //select过后的套接字
	struct sockaddr_in server;
	struct sockaddr_in client;
	int maxfd;
	int sockfd;
	int confd;
	char recvbuf[BUFSIZE];
	char sendbuf[BUFSIZE];
	int recvnum;
	int sendnum;
	int opt;   //定义套接字属性
	int length;   //用于connect函数

	opt = SO_REUSEADDR;
	length = sizeof(struct sockaddr);

	int tmp_i;
	int tmp_j;
	char str1[256];
	char str2[256];
	char str3[256];
	int tmpid=-1;   //用于进行实际处理的套接字，在关系上是通过tmpfd得到的
	int tmpfd=-1;   //用来在新一轮循环中替换掉tmp_i，使得tmp_i的信息可以保存
	struct client_info clientinfo[BACKLOG];

	//初始化套接字集合
	FD_ZERO(&allset);
	FD_ZERO(&rset);

	if(-1 == (sockfd=socket(AF_INET,SOCK_STREAM,0)))
	{
		perror("create socket error!\n");
		exit(1);
	}

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	memset(&server,0,sizeof(server));
	memset(sendbuf,0,BUFSIZE);
	memset(recvbuf,0,BUFSIZE);
	int i;
	//初始化客户的信息
	for(i=0;i<BACKLOG;i++)
	{
		clientinfo[i].id = -1;
		clientinfo[i].name[0] = '\0';
		clientinfo[i].first = -1;
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	if(-1 == bind(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))
	{
		perror("bind socket error!\n");
		exit(1);
	}

	if(-1 == listen(sockfd,BACKLOG))
	{
		perror("listen error!\n");
		exit(1);
	}

	FD_SET(sockfd,&allset);
	maxfd = sockfd;
	printf("server is ok!\n");

	while(1)
	{
		rset = allset;
		if(-1 == select(maxfd+1,&rset,NULL,NULL,NULL))
		{
			perror("select function error!\n");
			exit(1);
		}

		for(tmp_i = sockfd;tmp_i <= maxfd;tmp_i++)
		{
			//处理：如果是监听套接字被激活
			if(FD_ISSET(tmp_i,&rset))
			{
				if(tmp_i == sockfd)
				{
					confd = accept(sockfd,(struct sockaddr*)&client,&length);
					if(confd == -1)
					{
						perror("accept error!\n");
						exit(1);
					}
					clientinfo[confd].id = confd;
					clientinfo[confd].first = 1;  //将first置为1，用于第一个接收包的名字

					FD_SET(confd,&allset);
					if(confd > maxfd)
						maxfd = confd;
				}
				else{
					//处理：如果是连接套接字被激活
					recvnum = read(tmp_i,recvbuf,sizeof(recvbuf));
					if(clientinfo[tmp_i].first == 1)  //由上，得到客户的名字
					{
						strcpy(clientinfo[tmp_i].name,recvbuf);
						clientinfo[tmp_i].first = -1;
					}
					if(0>recvnum)
					{
						perror("recieve error!\n");
						exit(1);
					}
					if(recvbuf[0]=='/')
					{
						//处理：以‘/’开始的接收包表示现在是指令
						if(strcmp(recvbuf,"/who\n")==0){
							//请求现在有哪些用户在线
							for(tmpfd = sockfd;tmpfd<=maxfd;tmpfd++)
							{
								if(FD_ISSET(tmpfd,&allset))
									strcat(sendbuf,clientinfo[tmpfd].name);
							}
							//因为只是当前输入“/who”指令的用户想要知道谁在线
							//只把内容返回给他/她，用continue重新新的一轮循环
							write(tmp_i,sendbuf,sizeof(sendbuf));
							continue;
						}
						if(strcmp(recvbuf,"/quit\n")==0)
						{
							//当前客户请求退出
							printf("client:%s exit!\n",clientinfo[tmp_i].name);
							FD_CLR(tmp_i,&allset);
							close(tmp_i);
							strcat(sendbuf,clientinfo[tmp_i].name);
							strcat(sendbuf," was exit!");
						}
						//初始化字符串，用于分别存储/send usr msg中的各个部分
						memset(str1,0,sizeof(str1));
						memset(str2,0,sizeof(str2));
						memset(str3,0,sizeof(str3));
						sscanf(recvbuf,"%s %s %s",str1,str2,str3);
						strcat(str2,"\n");
						if(strcmp(str1,"/send")==0)
						{
							tmpid = -1;  //以防在新的循环中tmpid的值被上一次循环所改变
							int j = 0;
							for(tmpfd = sockfd;tmpfd<=maxfd;tmpfd++)
							{
								//查询到指定名字下的客户的套接字
								if(FD_ISSET(tmpfd,&allset))
								{
									if(strcmp(str2,clientinfo[tmpfd].name)==0)
										tmpid = tmpfd;
								}
							}
							if(tmpid==-1)
							{
								//表示并没有当前客户与之匹配，返回消息给发送端
								strcat(sendbuf,"user isn't online!");
								write(tmp_i,sendbuf,sizeof(sendbuf));
								continue;
							}
							strcat(sendbuf,clientinfo[tmp_i].name);
							strcat(sendbuf,str3);
							//因为这里是点对点，所以不用进入下面的部分，continue跳过
							write(tmpid,sendbuf,sizeof(sendbuf));
							continue;
						}
					}
					else
					{
						strcat(sendbuf,clientinfo[tmp_i].name);
						strcat(sendbuf," said: ");
						strcat(sendbuf,recvbuf);
					}
					for(tmp_j = sockfd+1; tmp_j<=maxfd;tmp_j++)
					{
						//实现信息的广播
						if(FD_ISSET(tmp_j,&allset))
						{
							write(tmp_j,sendbuf,strlen(sendbuf));
						}
					}

				}
			}
		}
		//清空sendbuf和recvbuf
		memset(&sendbuf,0,BUFSIZE);
		memset(&recvbuf,0,BUFSIZE);
	}

	return 0;
}


