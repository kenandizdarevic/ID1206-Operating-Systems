#include <pthread.h>    /* Threading interfaces */
#include <stdio.h>      /* Standard input & output */ 
#include <unistd.h>     /* Acess to POSIX API*/
#include <stdlib.h>     /* MALLOC */

// Initialize global variables & mutex
int buf = 0;
int MAX_BUF_MODIFICATION = 15;
pthread_mutex_t lock;

/// @brief Prints out TID, PID & buffer value. Increments buffer.
/// @param args *thread
/// @return Integer representing how much the buffer was incremented.
void* thread_func(void* args) {
    /*  Put the value on the heap so that
        the value is valid after the thread
        has exited.                          */
    int* mod = malloc(sizeof(int));

    while(1) {

        if (buf >= MAX_BUF_MODIFICATION) 
            break;

        // Locks the mutex
        pthread_mutex_lock(&lock);
    
        printf("TID: %lu, PID: %d, Buffer: %d\n", pthread_self(), getpid(), buf++);
        (*mod)++;
        // Unlcoks the mutex
        pthread_mutex_unlock(&lock);
        
        // Use sleep so that other processes can enter thread_func
        sleep(1);
        
    }
    pthread_exit(mod);
}

int main() {
    // Initialize variables
    pthread_t threads[3];
    int total = 0;
    void *threadMod = NULL;
    
    pthread_mutex_init(&lock, NULL);

    // Create threads, send them to thread_func
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }

    // Wait for threads to terminate
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], &threadMod);
        
        // Cast and dereference threadMod to access the correct value
        printf("TID %lu worked on the buffer %d times\n", threads[i], *((int*)threadMod));
        total += *((int*)threadMod);
        free(threadMod);
    }

    pthread_mutex_destroy(&lock);
    
    printf("Total buffer accesses: %d\n", total);

    return 0;
}