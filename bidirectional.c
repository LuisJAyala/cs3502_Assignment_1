#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {

    int pipe1[2]; 
    int pipe2[2]; 
    pid_t pid;
    char buffer[100];
	if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation unsuccesful");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("Fork unsuccesful");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
       memset(buffer, 0, sizeof(buffer));
        read(pipe1[0], buffer, sizeof(buffer) - 1);
        printf("Child received: %s\n", buffer);
        char *child_response = "Hi from child!";
        write(pipe2[1], child_response, strlen(child_response) + 1);

        close(pipe1[0]);
        close(pipe2[1]);
     exit(EXIT_SUCCESS);
    } else {

        close(pipe1[0]);
        close(pipe2[1]);
 
      char *parent_message = "Hi from parent!";
        printf("Parent sending: %s\n", parent_message);
        write(pipe1[1], parent_message, strlen(parent_message) + 1);

        
        memset(buffer, 0, sizeof(buffer));
        read(pipe2[0], buffer, sizeof(buffer) - 1);
        printf("Parent received back: %s\n", buffer);// Clean up remaining open ends
        close(pipe1[1]);
        close(pipe2[0]);

        
        wait(NULL);
    }
    return 0;
}

