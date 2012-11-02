#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
	pid_t pid;

 	printf("this program was executed before fork\n");


	pid = fork();

	if(0>pid)
	{
		perror("fork error\n");
		exit(1);

	}
	else if(0==pid)
	{
		fprintf(stdout,"this message is printed by parent\n");
		exit(0);

	}
	else
	{
		fprintf(stdout,"this message is printed by child\n");
		exit(0);

	}




}
