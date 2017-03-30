#include <stdio.h>
#include <pthread.h>
pthread_mutex_t mutex;
pthread_cond_t condc, condp;
typedef struct 
{
	pthread_t evm[5]; // each booth has 5 evm's
	int buffer[5];
}booth;
booth booths[3];
pthread_t voters[15];
int buffer = 0;
int no_booths = 3,no_voters =15, no_evms = 5,no_slots = 2;
void* polling_ready_booth(void *ptr) 
{
	int i;
	for (i = 1; i <= no_voters; i++) 
	{
	pthread_mutex_lock(&mutex);	/* protect buffer */
	pthread_cond_wait(&condp, &mutex);
	while (buffer != 0)	
		pthread_cond_wait(&condp, &mutex);
	buffer = i;
	pthread_cond_signal(&condc);	/* wake up consumer */
	pthread_mutex_unlock(&mutex);	/* release the buffer */
	}
	pthread_exit(0);
}
void* consumer(void *ptr) 
{
	int i;
  for (i = 1; i <= no_evms; i++) {
	pthread_mutex_lock(&mutex);	
	while (buffer == 0)			
			pthread_cond_wait(&condc, &mutex);
		buffer = 0;
		pthread_cond_signal(&condp);	/* wake up consumer */
		pthread_mutex_unlock(&mutex);	/* release the buffer */
  }
	pthread_exit(0);
}

int main(int argc, char **argv) 
{
	int i,j;
	pthread_mutex_init(&mutex, NULL);	
	pthread_cond_init(&condc, NULL);		/* Initialize consumer condition variable */
	pthread_cond_init(&condp, NULL);		/* Initialize producer condition variable */
	for(i=0;i<no_voters;i++)
	
	pthread_create(&voters[i], NULL, polling_ready_booth, NULL);
	
	for(i=0;i<no_booths;i++)
	for(j=0;j<no_evms;j++)
		pthread_create(&booths[i].evm[j], NULL, consumer, NULL);
	
	for(i=0;i<no_voters;i++)
	
		pthread_join(voters[i], NULL);
	
	for(i=0;i<no_booths;i++)
	
		for(j=0;j<no_evms;j++)
	
			pthread_join(booths[i].evm[j], NULL);
	
	
	pthread_mutex_destroy(&mutex);	/* Free up mutex */
	pthread_cond_destroy(&condc);		/* Free up consumer condition variable */
	pthread_cond_destroy(&condp);		/* Free up producer condition variable */
}
