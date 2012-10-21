// tcp_client.c

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/in.h>
#include <signal.h>


#define BUF_SIZE 2048
#define PORT 8964

int main(int argc, char const *argv[])
{
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	struct sockaddr_in server_addr;
	int err, mysocket, len;


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

	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	// 将用户输入的字符串类型的IP地址转为整型
	connect(mysocket, (struct sockaddr *) &server_addr,
		sizeof(struct sockaddr));

	for (;;) {
		write(1, "\nTCP>", 6);
		// 从标准输入中读取数据到缓冲区send_buf中
		fgets(send_buf, sizeof(send_buf), stdin);
		// 判断是否退出
		if (strcmp(send_buf, "quit\n") == 0) {
			printf("Bye-bye\n");
			break;
		}
		len = sizeof(send_buf);
		if (len > 0) {
			send(mysocket, send_buf, sizeof(send_buf), 0);
			len =
			    recv(mysocket, recv_buf, sizeof(recv_buf), 0);

			recv_buf[len] = '\0';
			printf("%s\n", recv_buf);
		}
	//	    bzero(recv_buf,sizeof(send_buf));
	}
	close(mysocket);

	return 0;
}
