#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>





volatile sig_atomic_t shutdown_flag = 0;
void handle_signals(int sig) {
    if (sig == SIGINT) {
        shutdown_flag = 1;
        printf("\n[Signal] SIGINT caught. Setting shutdown flag to 1...\n");
    } else if (sig == SIGUSR1) {

        printf("\n[Signal] SIGUSR1 caught. [Stats]: Pipeline running smoothly.\n");

    }


int main() {


    int pipe1[2]; 
    int pipe2[2]; 
    pid_t pid;
    char buffer[100];
    struct sigaction sa;

    sa.sa_handler = handle_signals;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);


    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation unsuccessful");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("Fork unsuccessful");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {

        close(pipe1[1]); 
        close(pipe2[0]); 


        while (!shutdown_flag) {
            memset(buffer, 0, sizeof(buffer));

            int bytes_read = read(pipe1[0], buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0) {

                break; 
            }

            printf("Child received: %s\n", buffer);


            char *child_response = "Hi from child!";
            if (write(pipe2[1], child_response, strlen(child_response) + 1) == -1) {
                perror("Child write failed");
                break;
            }
        }


        close(pipe1[0]);
        close(pipe2[1]);
        printf("[Child] Exiting gracefully.\n");
        exit(EXIT_SUCCESS);

    } else {

        close(pipe1[0]); 
        close(pipe2[1]); 

        char *parent_message = "Hi from parent!";
        

        while (!shutdown_flag) {
            printf("Parent sending: %s\n", parent_message);
            
            if (write(pipe1[1], parent_message, strlen(parent_message) + 1) == -1) {
                perror("Parent write failed");
                break;
            }

            memset(buffer, 0, sizeof(buffer));

            if (read(pipe2[0], buffer, sizeof(buffer) - 1) > 0) {
                printf("Parent received back: %s\n", buffer);
            }

            sleep(2); 
        }

       close(pipe1[1]);
        close(pipe2[0]);

        wait(NULL);
        printf("[Parent] Exiting gracefully.\n");
    }

    return 0;
}
