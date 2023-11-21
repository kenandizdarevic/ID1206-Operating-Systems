#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define MAX 10
#define MAX_READERS 2
const char *mutex = "mutex";
const char *wrt = "wrt";

typedef struct {
    int var;
    int read_count;
} SharedMemory;

void writer(SharedMemory* shared_memory) {
    sem_t *sem_wrt = sem_open(wrt, O_CREAT, 0600, 1);
    do {
        sem_wait(sem_wrt);
        printf("The writer acquires the lock.\n");
        shared_memory->var++;
        printf("The writer (%d) writes the value %d\n", getpid(), shared_memory->var);
        printf("The writer releases the lock.\n");
        sem_post(sem_wrt);
        sleep(2); // Delay for visibility
    } while(shared_memory->var < MAX);

    //sem_close(mtx);
    sem_close(sem_wrt);
    exit(EXIT_SUCCESS);
}

void reader(SharedMemory* shared_memory, int reader_id) {
    sem_t *sem_mutex = sem_open(mutex, O_CREAT, 0600, 1);
    sem_t *sem_wrt = sem_open(wrt, O_CREAT, 0600, 1);
    do {
        sem_wait(sem_mutex);
        shared_memory->read_count++;
        if (shared_memory->read_count == 1) {
            sem_wait(sem_wrt);
            printf("The first reader acquires the lock.\n");
        }
        sem_post(sem_mutex);

        printf("Reader (%d) reads the value %d\n", getpid(), shared_memory->var);

        sem_wait(sem_mutex);
        shared_memory->read_count--;
        if (shared_memory->read_count == 0) {
            printf("The last reader releases the lock.\n");
            sem_post(sem_wrt);
        }
        sem_post(sem_mutex);

        sleep(2); // Delay for visibility
    } while(shared_memory->var < MAX);

    sem_close(sem_mutex);
    sem_close(sem_wrt);
    exit(EXIT_SUCCESS);
}

int main() {
    int shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedMemory));
    SharedMemory *shared_memory = (SharedMemory *)mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shared_memory->var = 0;
    shared_memory->read_count = 0;

    pid_t writer_pid = fork();

    if (writer_pid == 0) { // Child
        writer(shared_memory);
    } else {           // Parent
        pid_t reader_pids[MAX_READERS];

        for (int i = 0; i < MAX_READERS; i++) {
            reader_pids[i] = fork();

            if (reader_pids[i] == 0) {
                reader(shared_memory, i+1);
            }
        }

        // Writer
        wait(NULL);

        // Readers
        wait(NULL);
        wait(NULL);
            
        // Remove shared memory and semaphore
        munmap(shared_memory, sizeof(SharedMemory));
        shm_unlink("/shared_memory");
    }

    return 0;
}

