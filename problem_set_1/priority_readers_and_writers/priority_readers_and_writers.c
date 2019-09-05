#include "priority_readers_and_writers.h"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_WRITER_THREADS 5
#define NUM_READER_THREADS 5
#define NUM_WRITES 5
#define NUM_READS 5

// Global shared variables
pthread_mutex_t gSharedLock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t gReadPhase = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWritePhase = PTHREAD_COND_INITIALIZER;
int gReaders = 0, gWaitingReaders = 0;
int gSharedValue = 0;

void *writerThreadFunc(void *pArg) {
    int id = *((int *) pArg);
    int numReaders = 0;

    for (int i = 0; i < NUM_WRITES; i++) {
        // Wait some random amount of time
        usleep(1000 * random() % (NUM_READER_THREADS + NUM_WRITER_THREADS));

        // Enter critical section
        pthread_mutex_lock(&gSharedLock);
            while(gReaders != 0) {
                pthread_cond_wait(&gWritePhase, &gSharedLock);
            }
            gReaders = -1;
            numReaders = gReaders;
        pthread_mutex_unlock(&gSharedLock);

        // Write data
        fprintf(stdout, "[Writer %d] writing %u* [Number of readers %d]\n", id, ++gSharedValue, numReaders);

        // Exit critical section
        pthread_mutex_lock(&gSharedLock);
            gReaders = 0;
            if (gWaitingReaders > 0) {
                pthread_cond_broadcast(&gReadPhase);
            }
            else {
                pthread_cond_signal(&gWritePhase);
            }
        pthread_mutex_unlock(&gSharedLock);
    }

    pthread_exit(0);
}

void *readerThreadFunc(void *pArg) {
    int id = *((int*) pArg);
    int numReaders = 0;

    for (int i = 0; i < NUM_READS; i++) {
        // Wait some random amount of time
        usleep(1000 * random() % (NUM_READER_THREADS + NUM_WRITER_THREADS));
        
        // Enter critical section
        pthread_mutex_lock(&gSharedLock);
        gWaitingReaders++;
            while (gReaders == -1) {
                pthread_cond_wait(&gReadPhase, &gSharedLock);
            }
        gWaitingReaders--;
        numReaders = ++gReaders;
        pthread_mutex_unlock(&gSharedLock);

        // Read data
        fprintf(stdout, "[Reader %d] reading %u [Number of readers: %d]\n", id, gSharedValue, numReaders);

        // Exit critical section
        pthread_mutex_lock(&gSharedLock);
            gReaders--;
            if (gReaders == 0) {
                pthread_cond_signal(&gWritePhase);
            }
        pthread_mutex_unlock(&gSharedLock);
    }

    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    int writerNums[NUM_WRITER_THREADS];
    int readerNums[NUM_READER_THREADS];

    pthread_t writers[NUM_WRITER_THREADS];
    pthread_t readers[NUM_READER_THREADS];

    for (int i = 0; i < NUM_WRITER_THREADS; i++) {
        writerNums[i] = i;
        pthread_create(&writers[i], NULL, writerThreadFunc, &writerNums[i]);
    }

    for (int i = 0; i < NUM_READER_THREADS; i++) {
        readerNums[i] = i;
        pthread_create(&readers[i], NULL, readerThreadFunc, &readerNums[i]);
    }

    for (int i = 0; i < NUM_WRITER_THREADS; i++) {
        pthread_join(writers[i], NULL);
    }

    for (int i = 0; i < NUM_READER_THREADS; i++) {
        pthread_join(readers[i], NULL);
    }
    
    return 0;
}