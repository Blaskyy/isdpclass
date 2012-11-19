#include <stdio.h>
#include <pthread.h>

struct arg
{
	char info;
	int num;
};

void * thread_info(void* arg)
{
	struct arg * parg;
	int count;

	parg = (struct arg*)arg;
	
	for(count=0;count<parg->num;count++)
		fprintf(stderr,"%c:%d\n",parg->info,count);

	pthread_exit(0);
	

}

int main()
{
	pthread_t thread1;
	pthread_t thread2;
	struct arg info1;
	struct arg info2;
	int ret;

	info1.info='L';
	info1.num = 4;
	info2.info='G';
	info2.num = 5;

	ret = pthread_create(&thread1,NULL,(void*)thread_info,(void*)&info1);
	if (0!=ret)
	{
		perror("create thread error\n");
		exit(1);

	}

	ret = pthread_create(&thread2,NULL,(void*)thread_info,(void*)&info2);
	if (0!=ret)
	{
		perror("create thread error\n");
		exit(1);

	}

	ret=pthread_join(thread1,NULL);
	if (0!=ret)
	{
		perror("waitting thread1 error\n");
		exit(1);
	}

	ret=pthread_join(thread2,NULL);
	if (0!=ret)
	{
		perror("waitting thread2 error\n");\
		exit(1);
	}

	exit(0);
	


}
