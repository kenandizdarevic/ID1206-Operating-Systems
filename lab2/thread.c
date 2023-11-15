#include <pthread.h>    /* Threading interfaces */
#include <stdio.h>      /* Standard input & output */ 
#include <unistd.h>     /* Acess to POSIX API*/

// Initialize global variables & mutex
int buf = 0;
pthread_mutex_t lock;

/// @brief Prints out TID, PID & buffer value. Increments buffer.
/// @param args *thread
/// @return Integer representing how much the buffer was incremented.
void* thread_func(void* args) {

    // Modify buffer 15 times
    while(1) {
        // Locks the mutex
        pthread_mutex_lock(&lock);
        if (buf < 15) {
            printf("TID: %lu, PID: %d, Buffer: %d\n", pthread_self(), getpid(), buf++);
            // Unlcoks the mutex
            pthread_mutex_unlock(&lock);
            
            // Use sleep so that other processes can enter thread_func
            sleep(1);
        }
    }
}

int main() {
    // Initialize variables
    pthread_t threads[3];

    int total = 0;
    
    pthread_mutex_init(&lock, NULL);

    // Create threads, send them to thread_func
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }

    // Wait for threads to terminate
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    return 0;
}