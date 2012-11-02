#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char **argv)
{
	char *ip;
	int startport, endport, sockfd, i;
	struct sockaddr_in to;
	struct servent *sp;
	if (4 != argc) {
		printf("usage:%s ip startport endport\n",argv[0]);
		return 0;
	}
	ip = argv[1];
	startport = atoi(argv[2]);
	endport = atoi(argv[3]);
	if (startport < 1 || endport > 65535 || endport < startport) {
		printf("端口范围错误\n");
		return 0;
	} else
		printf("IP:%s %d-%d\n", ip, startport, endport);
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = inet_addr(ip);
	for (i = startport; i <= endport; i++) {
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		to.sin_port = htons(i);
		if (connect
		    (sockfd, (struct sockaddr *) &to,
		     sizeof(struct sockaddr)) == 0) {
            sp = getservbyport(to.sin_port, NULL);
			if (sp != NULL) {
				printf("%s:%d,serv:%s\n", ip, i, sp->s_name);
			} else {
				printf("%s:%d,serv:Unknow\n", ip, i);
			}
		}
		close(sockfd);
	}
	return 0;
}
