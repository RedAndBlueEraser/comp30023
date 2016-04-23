/*
 * producer-consumer-problem.c
 * Version 20160423
 * Written by Harry Wong
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define FALSE               0
#define TRUE                1
#define MAX_RAND_SLEEP_TIME 3

typedef int buffer_item;
#define BUFFER_SIZE 5

buffer_item buffer[BUFFER_SIZE];
int buffer_space = BUFFER_SIZE;
pthread_mutex_t buffer_lock;

int buffer_has_space();
int buffer_has_item();
int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *producer(void *param);
void *consumer(void *param);

int buffer_has_space()
{
    return buffer_space > 0;
}

int buffer_has_item()
{
    return buffer_space < BUFFER_SIZE;
}

int insert_item(buffer_item item)
{
    /* insert an item into buffer */
    /* what happens if space is not available? */
    /* return 0 if successful, otherwise
    return -1 if unsuccessful */
    int buf_has_space,  // Does the buffer has space to insert?
        buf_pos;        // Position in buffer to insert to.

    while (!buffer_has_space());

    pthread_mutex_lock(&buffer_lock);

    if ((buf_has_space = buffer_has_space()))
    {
        buf_pos = BUFFER_SIZE - buffer_space;

        buffer[buf_pos] = item;
        buffer_space--;
    }

    pthread_mutex_unlock(&buffer_lock);

    return buf_has_space ? 0 : -1;
}

int remove_item(buffer_item *item)
{
    /* remove an item from the buffer */
    /* what happens if the buffer is empty */
    /* return 0 if successful, otherwise
    return - 1 if unsuccessful */
    int i,
        buf_has_item,  // Is the buffer full?
        buf_pos;      // Last occupied position in buffer.

    while (!buffer_has_item());

    pthread_mutex_lock(&buffer_lock);

    if ((buf_has_item = buffer_has_item()))
    {
        buf_pos = BUFFER_SIZE - buffer_space - 1;

        *item = buffer[0];
        buffer_space++;

        // Push items up buffer.
        for (i = 0; i < buf_pos; i++)
        {
            buffer[i] = buffer[i + 1];
        }
    }

    pthread_mutex_unlock(&buffer_lock);

    return buf_has_item ? 0 : -1;
}

int main(int argc, char *argv[])
{
    int i, exec_duration, n_producer_threads, n_consumer_threads;
    pthread_t *producer_threads, *consumer_threads;

    /* 1. get cmd line arguments */
    if (argc < 4)
    {
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }
    else
    {
        exec_duration      = atoi(argv[1]);
        n_producer_threads = atoi(argv[2]);
        n_consumer_threads = atoi(argv[3]);
    }

    /* 2. Initialize buffer */
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = 0;
    }
    if (pthread_mutex_init(&buffer_lock, NULL) != 0)
    {
        perror("pthread_mutex_init");
        return 1;
    }

    /* 3. Create producer threads */
    producer_threads = (pthread_t*)malloc(sizeof(pthread_t) * n_producer_threads);
    if (producer_threads == NULL)
    {
        perror("malloc");
        return 1;
    }
    for (i = 0; i < n_producer_threads; i++)
    {
        if (pthread_create(&producer_threads[i], NULL, producer, NULL) != 0)
        {
            perror("pthread_create");
            return 1;
        }
    }

    /* 4. Create consumer threads */
    consumer_threads = (pthread_t*)malloc(sizeof(pthread_t) * n_consumer_threads);
    if (consumer_threads == NULL)
    {
        perror("malloc");
        return 1;
    }
    for (i = 0; i < n_consumer_threads; i++)
    {
        if (pthread_create(&consumer_threads[i], NULL, consumer, NULL) != 0)
        {
            perror("pthread_create");
            return 1;
        }
    }

    /* 5. Sleep */
    sleep(exec_duration);

    /* 6. Exit */
    /*for (i = 0; i < n_producer_threads; i++)
    {
        if (pthread_join(producer_threads[i], NULL) != 0)
        {
            perror("pthread_join");
            return 1;
        }
    }
    for (i = 0; i < n_consumer_threads; i++)
    {
        if (pthread_join(consumer_threads[i], NULL) != 0)
        {
            perror("pthread_join");
            return 1;
        }
    }
    pthread_exit(NULL);*/
    if (pthread_mutex_destroy(&buffer_lock) != 0)
    {
        perror("pthread_mutex_destroy");
        return 1;
    }
    return 0;
}

void *producer(void *param)
{
    buffer_item item;
    while (TRUE)
    {
        /* sleep a random time period */
        sleep(rand() % MAX_RAND_SLEEP_TIME + 1);
        /* generate a random item */
        item = rand();
        if (insert_item(item) == -1)
        {
            fprintf(stderr,"Buffer is full\n");
        }
        else
        {
            printf("Producer produced %d\n",item);
        }
    }
}

void *consumer(void *param)
{
    buffer_item item;
    while (TRUE)
    {
        /* sleep a random time period */
        sleep(rand() % MAX_RAND_SLEEP_TIME + 1);
        /* generate a random item */
        if (remove_item(&item) == -1)
        {
            fprintf(stderr,"Buffer is empty\n");
        }
        else
        {
            printf("Consumer consumed %d\n",item);
        }
    }
}
