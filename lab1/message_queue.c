#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <mqueue.h>

#define MAX_SIZE 10000

int word_count(char buffer[]){
    int word_count = 0;
    char *token = strtok(buffer, " \t\n");

    while (token != NULL) {
        word_count++;
        token = strtok(NULL, " \t\n");
    }
    return word_count;
}

void child_process() {
    // Open the message queue for reading
    mqd_t mq = mq_open("/my_queue", O_RDONLY);

    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_SIZE];

    // Receive message from the message queue
    ssize_t received = mq_receive(mq, buffer, 10000, NULL);

    if (received == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }

    printf("Number of words: %d\n", word_count(buffer));

    // Close the message queue
    mq_close(mq);
    exit(EXIT_SUCCESS);
}

void parent_process() {
    // Open the message queue for writing
    mqd_t mq = mq_open("/my_queue", O_WRONLY | O_CREAT, 0644, NULL);

    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen("message.txt", "r");

    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Read the content of the file
    char buffer[MAX_SIZE];
    size_t bytesRead = fread(buffer, 1, MAX_SIZE, file);

    // Close the file
    fclose(file);

    // Send the content to the message queue
    if (mq_send(mq, buffer, bytesRead, 0) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }

    // Close the message queue
    mq_close(mq);
}

int main() {
    switch (fork())
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        break;
    case 0:
        child_process();
        break;
    
    default:
        parent_process();
        wait(NULL);
        break;
    }
}
