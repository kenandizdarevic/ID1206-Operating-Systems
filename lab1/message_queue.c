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

    int MAX_NUM_MSG = 1000;
    int MAX_SIZE = 1000;

    char buffer[MAX_SIZE];
    char read[MAX_SIZE];

    // Message queue
    char *my_mq = "/mymq";
    mqd_t mqd;

    struct mq_attr attr = {
        .mq_maxmsg = MAX_NUM_MSG,
        .mq_msgsize = MAX_SIZE
    };

    switch (fork()) {
        case -1: // fork error
            break;
        case 0: // Producer
            FILE *file = fopen("message.txt", "r");
            fgets(buffer, MAX_SIZE, file);
            
            // Open the global message queue
            mqd = mq_open(my_mq, O_CREAT | O_WRONLY, 0666, &attr);

            printf("Message: %s\n", buffer);

            // Write file to the message queue
            if (mq_send(mqd, buffer, strlen(buffer) + 1, 0) == 0)
            {
                printf("Message sent successfully...\n");
            }
            else
            {
                printf("Message failed to send!!!\n");
            }

            

            // Close file and message queue
            fclose(file);
            mq_close(mqd);
            exit(1);
            break;
        default: // Consumer
            wait(&stat);
            if(WIFEXITED(stat)) {
                
                mqd = mq_open(my_mq, O_RDONLY);
                mq_receive(mqd, read, MAX_NUM_MSG, NULL);
                printf("Message: %s\n", read);
                mq_close(mqd);

            }
            break;
    }
    return 0;
}
