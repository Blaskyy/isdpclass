#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc,char**argv)
{
	pid_t pid;
	
	pid = fork();
	
	if (0>pid)
	{
		perror("fork error\n");
		exit(1);
	}
	else if (0<pid)
	{
		printf("I am parent process\n");
		wait(0);
		_exit(0);
	}
	else 
	{
		sleep(10);
		execlp("ls","ls",argv[1],(char*)NULL);
		_exit(0);

	}

}

