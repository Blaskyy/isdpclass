#include <stdio.h>
#include <pthread.h>

void * thread_info()
{

	fprintf(stderr,"i am a pthread\n");
	pthread_exit(NULL);

}

int main()
{
	pthread_t thread;

	thread=pthread_create(&thread,NULL,(void*)thread_info,NULL);
	if (0!=thread)
	{
		fprintf(stderr,"create pthread error\n");
		exit(1);

	}
	exit(0);


}
