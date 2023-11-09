#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int status;

    // fd[0] read from pipe
    // fd[1] write to pipe
    int fd[2];

    // Create pipe
    pipe(fd);

    // Create fork(), parent & child
    switch (fork()) {
        case -1: // Fork error
            break;
        case 0: // Child
            close(fd[0]);

            dup2(fd[1], STDOUT_FILENO);

            execlp("/bin/ls", "ls", "/", NULL);
            
            printf("Child failed %s");
            exit(1);
            break;
        default: // Parent
            // Wait until child process is finished
            wait(&status);
            // Check that child is successfull
            if(WIFEXITED(status)) {
                close(fd[1]);

                dup2(fd[0], STDIN_FILENO);


                execlp("/bin/wc", "wc", "-l", NULL);
                printf("Parent failed %s");
                exit(1);
            }
            break;
    }
    return 0;
}