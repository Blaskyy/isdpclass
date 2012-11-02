#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8964
#define MAX_SIZE 2048

int main(int argc, char **argv)
{
	struct sockaddr_in server;
	int port;
	int sockfd;
	int sendnum;
	int recvnum;
	char send_buf[MAX_SIZE];
	char recv_buf[MAX_SIZE];

	if (argc != 2) {
		printf("\n");
		exit(-1);
	}

	port = PORT;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create socket error\n");
		exit(-1);
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);

	if (connect
	    (sockfd, (struct sockaddr *) &server,
	     sizeof(struct sockaddr)) < 0) {
		printf("connect error\n");
		close(sockfd);
		exit(1);
	}

	while (1) {
		memset(send_buf, 0, MAX_SIZE);
		memset(recv_buf, 0, MAX_SIZE);

		printf("say to tcp server>");
		fgets(send_buf, 2045, stdin);
		printf("\n");

		if (send(sockfd, send_buf, 2045, 0) < 0) {
			printf("send data error\n");
			close(sockfd);
			exit(-1);
		}

		if (strcmp(send_buf, "quit\n") == 0) {
			printf("quit server\n");
			break;
		}

		if ((recvnum = recv(sockfd, recv_buf, 2045, 0)) < 0) {
			printf("recive data error\n");
			close(sockfd);
			exit(-1);
		}

		recv_buf[recvnum] = '\0';
		fprintf(stdout, "%s\n", recv_buf);
	}
	close(sockfd);
	exit(1);
}
