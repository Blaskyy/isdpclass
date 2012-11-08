#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#define PORT 8900
#define BUFSIZE 2048
int execute(char *command, char *buf)
{
	FILE *fp;
	int count;
	char commandbuf[2056];
	if ((NULL == command) || (NULL == buf)) {
		perror("command or buf is empty\n");
		return -1;
	}
	count = 0;
	memset(commandbuf, 0, 2056);
	strcat(commandbuf, "sh -c ");
	strcat(commandbuf, command);
	fprintf(stderr, "the command is %s\n", commandbuf);
	if (NULL == (fp = popen(commandbuf, "r"))) {
		perror("create pipe error\n");
		return -1;
	}
	while ((count < 2047) && (EOF != (buf[count++] = fgetc(fp))));
	buf[count - 1] = '\0';
	return count;
}
int main()
{
	int sockfd;
	int conn_sock;
	char sendbuf[BUFSIZE];
	char recvbuf[BUFSIZE];
	int sendnum;
	int recvnum;
	int length;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int opt;
	int cnt;
	memset(&client, 0, sizeof(client));
	memset(&server, 0, sizeof(server));
	memset(sendbuf, 0, BUFSIZE);
	memset(recvbuf, 0, BUFSIZE);
	length = 0;
	sockfd = -1;
	conn_sock = -1;
	opt = SO_REUSEADDR;
	if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("create socket error\n");
		return -1;
	}
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);
	if (-1 ==
	    bind(sockfd, (struct sockaddr *) &server, sizeof(server))) {
		perror("bind socket error\n");
		close(sockfd);
		return -1;
	}
	if (-1 == listen(sockfd, 10)) {
		perror("listen socket error\n");
		close(sockfd);
		return -1;
	}
	while (1) {
		if (-1 ==
		    (conn_sock =
		     accept(sockfd, (struct sockaddr *) &client,
			    &length))) {
			perror("three shakehands error\n");
			close(sockfd);
			return -1;
		}
		pid_t cpid;
		if (cpid = fork() == 0) {
			while (1) {
				memset(recvbuf, 0, BUFSIZE);
				memset(sendbuf, 0, BUFSIZE);
				if (0 >=
				    (recvnum =
				     read(conn_sock, recvbuf, BUFSIZE))) {
					perror("the commucation error\n");
					close(conn_sock);
					close(sockfd);
					return -1;
				}
				recvbuf[recvnum] = '\0';
				fprintf(stderr, "the command is:%s\n",
					recvbuf);
				if (0 == strcmp(recvbuf, "quit")) {
					fprintf(stderr,
						"the client is quit\n");
					close(conn_sock);
					break;
				}
				if (1 >= (cnt = execute(recvbuf, sendbuf))) {
					sprintf(sendbuf,
						"the invalid command,please try again\n");
				}
				fprintf(stderr, "the result is \n%s",
					sendbuf);
				if (0 >=
				    (sendnum =
				     write(conn_sock, sendbuf,
					   strlen(sendbuf)))) {
					perror("the commucation error\n");
					close(sockfd);
					close(conn_sock);
					return -1;
				}
			}
		} else if (cpid > 0) {
			close(conn_sock);
			continue;
		}
	}
	close(sockfd);
}
