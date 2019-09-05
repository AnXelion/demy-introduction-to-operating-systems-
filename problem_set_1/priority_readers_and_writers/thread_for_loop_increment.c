#include <stdio.h>
#include <pthread.h>

#define NUM_THREAD 99

int nums = 0;
int j = 0;
pthread_mutex_t gSharedLock = PTHREAD_MUTEX_INITIALIZER;


void *threadFunc(void *pArg) {
    pthread_mutex_lock(&gSharedLock);
        for (; j < 999999999; j++) {
            nums++;
        }
    pthread_mutex_unlock(&gSharedLock);

    pthread_exit(0);
}

int main(int argc, char *argv[]) {

    int threadID[NUM_THREAD];
    pthread_t threads[NUM_THREAD];

    for (int i = 0; i < NUM_THREAD; i++) {
        threadID[i] = i;
        pthread_create(&threads[i], NULL, threadFunc, &threadID[i]);
    }

    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("nums: %d\n", nums);
    
    return 0;
}