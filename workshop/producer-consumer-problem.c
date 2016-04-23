/*
 * my-stat.c
 * Version 20160411
 * Written by Harry Wong
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TRUE 1

typedef int buffer_item;
#define BUFFER_SIZE 5

buffer_item buffer[BUFFER_SIZE];
int buffer_space = BUFFER_SIZE;
int buffer_producer_pointer = 0;
int buffer_consumer_pointer = 0;

int insert_item(buffer_item item)
{
    /* insert an item into buffer */ /* what happens if space is not available? */
	/* return 0 if successful, otherwise return -1 if unsuccessful */
	if (buffer_space <= 0)
	{
		return -1;
	}
	else
	{
		buffer[buffer_producer_pointer] = item;
		buffer_producer_pointer = (buffer_producer_pointer + 1) % BUFFER_SIZE;
		return 0;
	}
}
int remove_item(buffer_item *item)
{
    /* remove an item from the buffer */ /* what happens if the buffer is empty */
    /* return 0 if successful, otherwise return - 1 if unsuccessful */
	if (buffer_space >= BUFFER_SIZE)
	{
		return -1;
	}
	else
	{
		*item = buffer[buffer_producer_pointer];
		buffer_consumer_pointer = (buffer_consumer_pointer + 1) % BUFFER_SIZE;
		return 0;
	}
}

void *producer(void *param)
{
    buffer_item item;
    while (TRUE)
    {
        /* sleep a random time period */
        sleep(rand()%(int)param);
        /* generate a random item */
        item = rand();
        if (insert_item(item) == -1)
        {
            fprintf(stderr,"Report error condition/n");
        }
        else
        {
            printf("Producer produced %d/n",item);
        }
    }
}

void *consumer(void *param)
{
    buffer_item item;
    while (TRUE)
    {
        /* sleep a random time period */
        sleep(rand()%(int)param);
        /* generate a random item */
        if (remove_item(&item) == -1)
        {
            fprintf(stderr,"Report error condition/n");
        }
        else
        {
            printf("Consumer consumed %d/n",item);
        }
    }
}

int main(int argc, char *argv[])
{
    int i, sleep_seconds, n_producer_threads, n_consumer_threads;
    pthread_t *producer_threads, *consumer_threads;

    /* 1. get cmd line arguments */
    if (argc < 4)
    {
        fprintf(stderr, "Not enough arguments\n");
        exit(1);
    }
    else
    {
        sleep_seconds       = atoi(argv[1]);
        n_producer_threads = atoi(argv[2]);
        n_consumer_threads = atoi(argv[3]);
    }

    /* 2. Initialize buffer */
	for (i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 0;

    /* 3. Create producer threads */
    producer_threads = (pthread_t*)malloc(sizeof(pthread_t) * n_producer_threads);
    for (i = 0; i < n_producer_threads; i++)
        pthread_create(&producer_threads[i], NULL, producer, (void*)sleep_seconds);

    /* 4. Create consumer threads */
    consumer_threads = (pthread_t*)malloc(sizeof(pthread_t) * n_consumer_threads);
    for (i = 0; i < n_consumer_threads; i++)
        pthread_create(&consumer_threads[i], NULL, consumer, (void*)sleep_seconds);

    /* 5. Sleep */
    sleep(sleep_seconds);

    /* 6. Exit */
    for (i = 0; i < n_producer_threads; i++)
        pthread_join(producer_threads[i], NULL);
    for (i = 0; i < n_consumer_threads; i++)
        pthread_join(consumer_threads[i], NULL);
    pthread_exit(NULL);
    return 0;
}
