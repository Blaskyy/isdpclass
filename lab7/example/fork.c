#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
	pid_t pid;
	
	pid = fork();
	printf("this program will be executed twice\n");


	return 0;

}
