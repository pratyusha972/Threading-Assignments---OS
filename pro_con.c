#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include <unistd.h>
int readd = 0,no_readers,sz_bf,len_wr,*ptr,counter=-1,status = 0;
pthread_mutex_t mutex;
sem_t full,empty;
void *producer();
void *consumer();
int main()
{
	int i,j;
	scanf("%d",&sz_bf);
	scanf("%d",&no_readers);
	scanf("%d",&len_wr);
	pthread_mutex_init(&mutex,NULL);
	sem_init(&full,0,0);
	sem_init(&empty,0,sz_bf);
	ptr = (int*) malloc(sz_bf * sizeof(int));
	for(j=0;j<len_wr;j++)
	{
		pthread_t writer;
		pthread_create(&writer,NULL,&producer,NULL);
		pthread_t *reader;
		reader = (pthread_t*) malloc(no_readers * sizeof(pthread_t)); 
		for(i=0;i<no_readers;i++)
			pthread_create(&reader[i],NULL,&consumer,NULL);
		pthread_join(writer,NULL);
		for(i=0;i<no_readers;i++)
		{
			pthread_join(reader[i],NULL);
		}
		sleep(2);
		for(i=0;i<sz_bf;i++)
			printf("%d ",ptr[i]);
		printf("\n");
		sleep(0.5);
	}
	return 0;	
}
void *producer()
{
	int data = rand()%100;
	if(status==0)
	{
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		readd = 1;
		printf("Producer has produced item: %d\n",data);
		counter = (counter+1)%sz_bf;
		ptr[counter] = data;
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
}
void *consumer()
{
	if(readd == 1)
	{	
		pthread_mutex_lock(&mutex);
		if(status==no_readers)
			sem_wait(&full);
		printf("Consumer has consumed item: %d\n",ptr[counter]);
		status++;
		if(status==no_readers)
		{
			sem_post(&empty);
			status = 0;
			readd = 0;
		}
		pthread_mutex_unlock(&mutex);
	}
}
