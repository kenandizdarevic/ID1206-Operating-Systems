#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

int main() {
    int BUFFER_SIZE = 25;
    char write_msg[] = "this is from the child\n";
    char read_msg[BUFFER_SIZE];
    int stat;
    int fd[2];

    int MAX_NUM_MSG = 10;
    int MAX_SIZE = 64;

    // Message queue
    char *my_mq = "/mymq";
    mqd_t mqd;

    struct mq_attr attr = {
        .mq_maxmsg = MAX_NUM_MSG,
        .mq_msgsize = MAX_SIZE
    };
    
    pipe(fd); // Create the pipe

    switch (fork()) {
        case -1: // fork error
            break;
        case 0: // Producer
            int file_desc = open("dup.txt", O_RDONLY);
            
            // Open the global message queue
            mqd = mq_open(my_mq, O_RDWR);
            // Write file to the message queue
            mq_send(mqd, write_msg, strlen(write_msg), 0);
            // Close the message queue
            mq_close(mqd);

            break;
        default: // Consumer
            wait(&stat);
            if(WIFEXITED(stat)) {
                
            }
            break;
    }
    return 0;
}
